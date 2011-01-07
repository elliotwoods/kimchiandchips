using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using VVVV.PluginInterfaces.V1;
using SlimDX.Direct3D9;
using System.Runtime.InteropServices;
using SlimDX;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;
using System.Diagnostics;

using xn;



namespace VVVV.Nodes.OpenNI
{

    public class DebugMessage
    {
        public DebugMessage(TLogType _Type, String _Text)
        {
            Type = _Type;
            Text = _Text;
        }
        public TLogType Type;
        public String Text;
    }

    public unsafe class OpenNInode : IPlugin, IDisposable, IPluginDXTexture
    {
        [DllImport("msvcrt.dll", EntryPoint = "memcpy")]
        public unsafe static extern void CopyMemory(IntPtr pDest, IntPtr pSrc, int length);

        #region Plugin Info
        public static IPluginInfo PluginInfo
        {
            get
            {
                IPluginInfo Info = new PluginInfo();
                Info.Name = "OpenNI";							//use CamelCaps and no spaces
                Info.Category = "EX9.Texture";						//try to use an existing one
                Info.Version = "";						//versions are optional. leave blank if not needed
                Info.Help = "Gets textures from OpenNI (generally for Kinect)";
                Info.Bugs = "";
                Info.Credits = "";								//give credits to thirdparty code used
                Info.Warnings = "";
                Info.Author = "sugokuGENKI";
                Info.Credits = "PrimeSense";

                //leave below as is
                System.Diagnostics.StackTrace st = new System.Diagnostics.StackTrace(true);
                System.Diagnostics.StackFrame sf = st.GetFrame(0);
                System.Reflection.MethodBase method = sf.GetMethod();
                Info.Namespace = method.DeclaringType.Namespace;
                Info.Class = method.DeclaringType.Name;
                return Info;
                //leave above as is
            }
        }
        #endregion

        #region Fields
        private IPluginHost FHost;
        private List<DebugMessage> DebugMessages = new List<DebugMessage>();
    
        //inputs
        private IStringIn FPinInConfig;
        private IValueIn FPinInPipetXY;
        private IValueIn FPinInBackgroundCapture;
        private IValueIn FPinInBackgroundThreshold;

        //outputs
        private IDXTextureOut FPinOutTextureDepth16;
        private IDXTextureOut FPinOutTextureXYZ;
        private IDXTextureOut FPinOutTextureRGB;

        private IValueOut FPinOutRunning;
        private IValueOut FPinOutPipetResultXYZ;

        public IValueOut FPinOutHands;

        private Dictionary<int, Texture> FTexturesD16 = new Dictionary<int, Texture>();
        private Dictionary<int, Texture> FTexturesXYZ = new Dictionary<int, Texture>();
        private Dictionary<int, Texture> FTexturesRGB = new Dictionary<int, Texture>();

        private bool FOutputEnabledD16 = false;
        private bool FOutputEnabledXYZ = false;
        private bool FOutputEnabledRGB = false;

        private IntPtr FDataDepth = new IntPtr();
        private IntPtr FDataXYZ = new IntPtr();
        private IntPtr FDataRGB = new IntPtr();

        //pipet
        private List<int> FPipetRequests = new List<int>();
        private bool FPipetRequestsReady = false; 
        
        private List<double[]> FPipetResults = new List<double[]>();
        private bool FPipetResultsReady = false;

        //Background
        private IntPtr  FBackgroundDepth = new IntPtr();
        private bool    FBackgroundCaptured = false;
        private UInt16  FBackgroundThreshold = 0;
        private bool    FBackgroundDoCopy = false;

        //OpenNI
        private int FWidth = 640;
        private int FHeight = 480;
               
        private string FConfigString = "";

        private bool FRunning = false;
        private Context FContext;
        private DepthGenerator FGeneratorDepth;
        private ImageGenerator FGeneratorImage;
        private HandsGenerator FGeneratorHands;
        private GestureGenerator FGeneratorGestures;
        private Thread readerThread;
        private bool shouldRun = false;

        private Dictionary<uint, Point3D> FHandsVector = new Dictionary<uint, Point3D>();
        #endregion

        #region Auto Evaluate
        public bool AutoEvaluate
        {
            get { return true; }
        }
        #endregion

        #region Set Plugin Host
        public void SetPluginHost(IPluginHost Host)
        {
            //assign host
            this.FHost = Host;

            //inputs
            this.FHost.CreateStringInput("Config", TSliceMode.Single, TPinVisibility.True, out FPinInConfig);

            this.FHost.CreateValueInput("Pipet", 2, null, TSliceMode.Dynamic, TPinVisibility.True, out FPinInPipetXY);
            FPinInPipetXY.SetSubType2D(0, 1, 0.001, 0, 0, false, false, false);

            this.FHost.CreateValueInput("Background capture", 1, null, TSliceMode.Single, TPinVisibility.True, out FPinInBackgroundCapture);
            FPinInBackgroundCapture.SetSubType(0, 1, 1, 0, true, false, false);
            this.FHost.CreateValueInput("Background threshold", 1, null, TSliceMode.Single, TPinVisibility.True, out FPinInBackgroundThreshold);
            FPinInBackgroundThreshold.SetSubType(0, 10, 0.001, 0, false, false, false);


            //outputs
            this.FHost.CreateTextureOutput("Depth 16", TSliceMode.Single, TPinVisibility.True, out this.FPinOutTextureDepth16);
            this.FHost.CreateTextureOutput("XYZ", TSliceMode.Single, TPinVisibility.True, out this.FPinOutTextureXYZ);
            this.FHost.CreateTextureOutput("RGB", TSliceMode.Single, TPinVisibility.True, out this.FPinOutTextureRGB);

            this.FHost.CreateValueOutput("Running", 1, null, TSliceMode.Single, TPinVisibility.True, out this.FPinOutRunning);
            this.FPinOutRunning.SetSubType(0, 1, 1, 0, false, true, false);

            this.FHost.CreateValueOutput("Pipet result", 3, null, TSliceMode.Dynamic, TPinVisibility.True, out FPinOutPipetResultXYZ);
            FPinOutPipetResultXYZ.SetSubType3D(-1, 1, 0.001, 0, 0, 0, false, false, false);

            //user outputs
            this.FHost.CreateValueOutput("Hands", 3, null, TSliceMode.Dynamic, TPinVisibility.True, out FPinOutHands);
            FPinOutHands.SetSubType3D(double.MinValue, double.MaxValue, 0.001, 0, 0, 0, false, false, false);

            Configurate(null);
        }
        #endregion

        #region Constructor
        public OpenNInode()
        {
            
        }
        ~OpenNInode()
        {
            stop();
        }
        #endregion

        #region Configurate
        public void Configurate(IPluginConfig Input)
        {
                        
        }
        #endregion

        #region Evaluate
        public void Evaluate(int SpreadMax)
        {
            //output debug messages
            foreach (DebugMessage Message in DebugMessages)
            {
                FHost.Log(Message.Type, Message.Text);
            }
            DebugMessages.Clear();

            //check config string
            if (FPinInConfig.PinIsChanged)
            {
                FPinInConfig.GetString(0, out FConfigString);
                if (FConfigString != null)
                    if (FConfigString.Length>0)
                        start();
            }

            //check texture connections
            FOutputEnabledD16 = this.shouldRun && FRunning && FPinOutTextureDepth16.IsConnected;
            FOutputEnabledXYZ = this.shouldRun && FRunning && FPinOutTextureXYZ.IsConnected;
            FOutputEnabledRGB = this.shouldRun && FRunning && FPinOutTextureRGB.IsConnected;

            //check running
            double Running = (FRunning ? 1 : 0);
            FPinOutRunning.SetValue(0, Running);
            
            //get pipet results
            if (FPipetResultsReady)
            {
                lock (this)
                {
                    int nPipets = FPipetResults.Count;
                    FPinOutPipetResultXYZ.SliceCount = nPipets;

                    for (int i = 0; i < nPipets; i++)
                        FPinOutPipetResultXYZ.SetValue3D(i, FPipetResults[i][0], FPipetResults[i][1], FPipetResults[i][2]);

                    FPipetResultsReady = false;
                }
            }

            //send pipet requests
            if (FPinInPipetXY.PinIsChanged)
            {
                int idx, idy, id;
                double px, py;
                lock (this)
                {
                    FPipetRequests.Clear();
                    for (int i = 0; i < FPinInPipetXY.SliceCount; i++)
                    {
                        FPinInPipetXY.GetValue2D(i, out px, out py);
                        if (px < 0) px = 0;
                        if (px > 1) px = 1;
                        if (py < 0) py = 0;
                        if (py > 1) py = 1;

                        idx = System.Convert.ToInt32(px * System.Convert.ToDouble(FWidth - 1));
                        idy = System.Convert.ToInt32(py * System.Convert.ToDouble(FHeight - 1));
                        id = idy * FWidth + idx;

                        FPipetRequests.Add(id);
                    }

                    FPipetRequestsReady = true;
                    FPipetResultsReady = false;
                }
            }

            //check if capture background
            if (FPinInBackgroundCapture.PinIsChanged && FRunning)
            {
                double currentValue;
                FPinInBackgroundCapture.GetValue(0, out currentValue);
                if (currentValue > 0.5)
                {
                    if (!FBackgroundCaptured)
                        FBackgroundDepth = Marshal.AllocCoTaskMem(FWidth * FHeight * 2);
 
                    FBackgroundDoCopy = true;
                }
                
            }

            if (FPinInBackgroundThreshold.PinIsChanged)
            {
                double currentValue;
                FPinInBackgroundThreshold.GetValue(0, out currentValue);

                FBackgroundThreshold = System.Convert.ToUInt16(currentValue * 1000.0);
            }

            //output hands
            FPinOutHands.SliceCount = FHandsVector.Count;
            int iHandOut = 0;
            foreach (KeyValuePair<uint, Point3D> HandMM in FHandsVector)
            {
                FPinOutHands.SetValue3D(iHandOut++, (double)HandMM.Value.X/1000.0f, (double)HandMM.Value.Y/1000.0f, (double)HandMM.Value.Z/1000.0f);
            }
        }
        #endregion

        #region Dispose
        public void Dispose()
        {
        }
        #endregion

        public void GetTexture(IDXTextureOut ForPin, int OnDevice, out int tex)
        {
            tex = 0;
            if (ForPin == this.FPinOutTextureDepth16)
            {
                if (this.FTexturesD16.ContainsKey(OnDevice))
                    tex = this.FTexturesD16[OnDevice].ComPointer.ToInt32();
            }

            if (ForPin == this.FPinOutTextureXYZ)
            {
                if (this.FTexturesXYZ.ContainsKey(OnDevice))
                    tex = this.FTexturesXYZ[OnDevice].ComPointer.ToInt32();
            }

            if (ForPin == this.FPinOutTextureRGB)
            {
                if (this.FTexturesRGB.ContainsKey(OnDevice))
                    tex = this.FTexturesRGB[OnDevice].ComPointer.ToInt32();
            }

        }

        public void DestroyResource(IPluginOut ForPin, int OnDevice, bool OnlyUnManaged)
        {
            if (this.FTexturesD16.ContainsKey(OnDevice))
            {
                this.FTexturesD16[OnDevice].Dispose();
                this.FTexturesD16.Remove(OnDevice);
            }

            if (this.FTexturesXYZ.ContainsKey(OnDevice))
            {
                this.FTexturesXYZ[OnDevice].Dispose();
                this.FTexturesXYZ.Remove(OnDevice);
            }

            if (this.FTexturesRGB.ContainsKey(OnDevice))
            {
                this.FTexturesRGB[OnDevice].Dispose();
                this.FTexturesRGB.Remove(OnDevice);
            }
        }

        public void UpdateResource(IPluginOut ForPin, int OnDevice)
        {
            if (!FRunning)
                return;

            SlimDX.Direct3D9.Device dev = SlimDX.Direct3D9.Device.FromPointer(new IntPtr(OnDevice));

            if (!this.FTexturesD16.ContainsKey(OnDevice))
            {
                Texture txt = new Texture(dev, FWidth, FHeight, 1, Usage.None, Format.L16, Pool.Managed);
                this.FTexturesD16.Add(OnDevice, txt);
            }

            if (!this.FTexturesXYZ.ContainsKey(OnDevice))
            {
                Texture txt = new Texture(dev, FWidth, FHeight, 1, Usage.None, Format.A32B32G32R32F, Pool.Managed);
                this.FTexturesXYZ.Add(OnDevice, txt);
            }

            if (!this.FTexturesRGB.ContainsKey(OnDevice))
            {
                Texture txt = new Texture(dev, FWidth, FHeight, 1, Usage.None, Format.X8R8G8B8, Pool.Managed);
                this.FTexturesRGB.Add(OnDevice, txt);
            }

            Texture tx = this.FTexturesD16[OnDevice];
            if (FOutputEnabledD16 && this.FDataDepth != null)
            {
                Surface srf = tx.GetSurfaceLevel(0);
                DataRectangle rect = srf.LockRectangle(LockFlags.Discard);
                rect.Data.WriteRange(this.FDataDepth, FWidth * FHeight * 2);
                srf.UnlockRectangle();
            }

            tx = this.FTexturesXYZ[OnDevice];
            if (FOutputEnabledXYZ && this.FDataXYZ != null)
            {
                Surface srf = tx.GetSurfaceLevel(0);
                DataRectangle rect = srf.LockRectangle(LockFlags.Discard);
                rect.Data.WriteRange(this.FDataXYZ, FWidth * FHeight * 4 * 4);
                srf.UnlockRectangle();
            }

            tx = this.FTexturesRGB[OnDevice];
            if (FOutputEnabledRGB && this.FDataRGB != null)
            {
                Surface srf = tx.GetSurfaceLevel(0);
                DataRectangle rect = srf.LockRectangle(LockFlags.Discard);
                rect.Data.WriteRange(this.FDataRGB, FWidth * FHeight * 4);
                srf.UnlockRectangle();
            }

        }
        
        private void start()
        {
            if (this.FRunning)
                stop() ;

            try
            {
                this.FContext = new Context();
                this.FContext.RunXmlScript(FConfigString);

                this.FRunning = true;

                //allocate to begin with. this memory is wasted until app resets
                //better idea is to not render texture until data is ready
                this.FDataDepth = Marshal.AllocCoTaskMem(FWidth * FHeight * 2);
                this.FDataRGB = Marshal.AllocCoTaskMem(FWidth * FHeight * 4);
                this.FDataXYZ = Marshal.AllocCoTaskMem(FWidth * FHeight * 4 * 4);

                //set alpha values to 1
                byte* RGB32 = (byte*) FDataRGB.ToPointer();
                for (int i = 0; i < FWidth * FHeight; i++, RGB32 += 4)
                    RGB32[0] = 255;
                
            }
            catch (XnStatusException e)
            {
                DebugMessages.Add(new DebugMessage(TLogType.Error, "OpenNI: " + e.Message));
                this.FRunning = false;
            }

            if (FRunning)
            {
                this.shouldRun = true;
            
                this.readerThread = new Thread(ReaderThread);
                this.readerThread.Start();

            }

        }

        private void stop()
        {
            if (this.FRunning)
            {
                Marshal.FreeCoTaskMem(FDataDepth);
                Marshal.FreeCoTaskMem(FDataRGB);
                Marshal.FreeCoTaskMem(FDataXYZ);

                this.FGeneratorGestures.StopGenerating();
            }

            if (this.shouldRun)
            {
                this.shouldRun = false;
                this.readerThread.Join();
            }
        }

        private unsafe void ReaderThread()
        {
            this.FGeneratorDepth = FContext.FindExistingNode(NodeType.Depth) as DepthGenerator;
            this.FGeneratorImage = FContext.FindExistingNode(NodeType.Image) as ImageGenerator;
            this.FGeneratorHands = FContext.FindExistingNode(NodeType.Hands) as HandsGenerator;
            this.FGeneratorGestures = FContext.FindExistingNode(NodeType.Gesture) as GestureGenerator;
            
            this.FGeneratorGestures.AddGesture("Click");
            this.FGeneratorGestures.AddGesture("Wave");
            this.FGeneratorGestures.AddGesture("RaiseHand");

            this.FGeneratorGestures.GestureRecognized += new GestureGenerator.GestureRecognizedHandler(FGeneratorGestures_GestureRecognized);
            this.FGeneratorHands.HandCreate +=new HandsGenerator.HandCreateHandler(FGeneratorHands_HandCreate);
            this.FGeneratorHands.HandUpdate +=new HandsGenerator.HandUpdateHandler(FGeneratorHands_HandUpdate);
            this.FGeneratorHands.HandDestroy +=new HandsGenerator.HandDestroyHandler(FGeneratorHands_HandDestroy);

            this.FGeneratorImage.SetPixelFormat(xn.PixelFormat.RGB24);

            if (this.FGeneratorDepth.IsCapabilitySupported("AlternativeViewPoint"))
                this.FGeneratorDepth.GetAlternativeViewPointCap().SetViewPoint(this.FGeneratorImage);

            while (this.shouldRun)
            {
                try
                {
                    this.FContext.WaitOneUpdateAll(this.FGeneratorDepth);
                }
                catch (XnStatusException e)
                {
                    DebugMessages.Add(new DebugMessage(TLogType.Error, "OpenNI: " + e.Message));
                }

                lock (this)
                {
                    if (FOutputEnabledD16 || FOutputEnabledXYZ)
                    {
                        CopyMemory(FDataDepth, this.FGeneratorDepth.GetDepthMapPtr(), FWidth * FHeight * 2);
                        if (FBackgroundDoCopy)
                        {
                            CopyMemory(FBackgroundDepth, FDataDepth, FWidth * FHeight * 2);
                            FBackgroundDoCopy = false;
                            FBackgroundCaptured = true;
                        }
                        
                        if (FBackgroundCaptured && FBackgroundDepth != IntPtr.Zero)
                        {
                            //presume here that 2's complement is MSB, and MSB=0
                            //need to have + and - for the abs difference line
                            short* DataArray = (short*)FDataDepth.ToPointer();
                            short* BackgroundArray = (short*)FBackgroundDepth.ToPointer();

                            short ShortZero = 0;
                            for (int i = 0; i < FWidth * FHeight; i++, DataArray++, BackgroundArray++)
                            {
                                if (Math.Abs(*DataArray - *BackgroundArray) < FBackgroundThreshold)
                                    DataArray[0] = ShortZero;
                            }
                        }

                        //FUsers.Update();
                    }

                    if (FOutputEnabledXYZ)
                        RenderXYZ();
                    if (FOutputEnabledRGB)
                    {
                        byte* RGB24 = (byte*) this.FGeneratorImage.GetImageMapPtr().ToPointer();
                        byte* RGB32 = (byte*) FDataRGB.ToPointer();

                        for (int i = 0; i < FWidth * FHeight; i++, RGB24 += 3, RGB32 += 4)
                        {
                            RGB32[0] = RGB24[2];
                            RGB32[1] = RGB24[1];
                            RGB32[2] = RGB24[0];
                        }
                    }
                }
            }
        }

        void  FGeneratorHands_HandDestroy(ProductionNode node, uint id, float fTime)
        {
            FHandsVector.Remove(id);
        }

        void  FGeneratorHands_HandUpdate(ProductionNode node, uint id, ref Point3D position, float fTime)
        {
            FHandsVector[id] = position;
        }

        void  FGeneratorHands_HandCreate(ProductionNode node, uint id, ref Point3D position, float fTime)
        {
 	        FHandsVector.Add(id, position);
        }

        void FGeneratorGestures_GestureRecognized(ProductionNode node, string strGesture, ref Point3D idPosition, ref Point3D endPosition)
        {
            DebugMessages.Add(new DebugMessage(TLogType.Message, "Gesture " + strGesture + " found at " + endPosition.ToString() + " id" + idPosition.ToString()));
            FGeneratorHands.StartTracking(ref endPosition);
        }

        private unsafe void RenderXYZ()
        {
            float* DataXYZ = (float*) FDataXYZ.ToPointer();
            ushort* DataDepth = (ushort*)FDataDepth.ToPointer();

            //calculate world positions
            for (int iY = 0; iY < FHeight; iY++)
                for (int iX = 0; iX < FWidth; iX++, DataXYZ += 4, DataDepth++)
                    DepthToWorld(iX, iY, DataDepth[0], DataXYZ);

            

            //check pipet requests
            if (FPipetRequestsReady)
            {
                //move pointer back to start
                DataXYZ = (float*)FDataXYZ.ToPointer();

                FPipetResults.Clear();
                for (int iPipet = 0; iPipet < FPipetRequests.Count; iPipet++)
                {
                    FPipetResults.Add(new double[3]);
                    FPipetResults[iPipet][0] = System.Convert.ToDouble(DataXYZ[FPipetRequests[iPipet]*4]);
                    FPipetResults[iPipet][1] = System.Convert.ToDouble(DataXYZ[FPipetRequests[iPipet]*4+1]);
                    FPipetResults[iPipet][2] = System.Convert.ToDouble(DataXYZ[FPipetRequests[iPipet]*4+2]);
                }

                FPipetRequestsReady = false;
                FPipetResultsReady = true;
            }
        }

        const float fx_d = 1.0f / 5.9421434211923247e+02f;
        const float fy_d = 1.0f / 5.9104053696870778e+02f;
        const float cx_d = 3.3930780975300314e+02f;
        const float cy_d = 2.4273913761751615e+02f;

        private unsafe void DepthToWorld(int x, int y, int depthValue, float *data)
        {
            //adapted from http://graphics.stanford.edu/~mdfisher/Kinect.html
            
            //presume we're in mm units
            data[2] = System.Convert.ToSingle(depthValue) / 1000.0f;
            data[0] = (System.Convert.ToSingle(x) - cx_d) * fx_d * data[2];
            data[1] = -(System.Convert.ToSingle(y) - cy_d) * fy_d * data[2];

            data[3] = (data[2] > 0.0f ? 1.0f : 0.0f);
        }

        private unsafe void DepthToWorld(int x, int y, int depthValue, float* data, int background)
        {
            //adapted from http://graphics.stanford.edu/~mdfisher/Kinect.html

            //presume we're in mm units
            data[2] = System.Convert.ToSingle(depthValue) / 1000.0f;
            data[0] = (System.Convert.ToSingle(x) - cx_d) * fx_d * data[2];
            data[1] = -(System.Convert.ToSingle(y) - cy_d) * fy_d * data[2];

            data[3] = ( (data[2] > 0.0f) &&
                        (Math.Abs(background - depthValue) > this.FBackgroundThreshold)
                        ? 1.0f : 0.0f);
        }
        }
    }

