//
//  ProcessMain.h
//  PC Encode
//
//  Created by Elliot Woods on 06/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxKCScreensGUI.h"
#include "ofxPolyFit.h"

#include <fstream>
class ProcessMain
{
    public:
        ProcessMain();
        ~ProcessMain();
    
        void            update();
    
        scrGroupGrid    scrGridMain;
    
    protected:
        void        allocate();
        void        deAllocate();

        void        allocateFiltered();
        void        deAllocateFiltered();
    
        void        load3DScan();
        void        filterRANSAC();
        void        saveFiltered();
    
        void        calcRGB();
    
        scrGroupGrid    scrData;
        scrPointCloud   scrRaw, scrFiltered;
        wdgButton       *bangLoadScan, *bangRANSAC, *bangSaveFiltered;
        scrWidgets      scrControl;
    
        float           ransac_selection;
        float           ransac_residual;
        ofxPolyFit      RANSAC;
    
    
        ///////////////////
        // DATA
        ///////////////////
    
        bool    allocated;
        unsigned int     nPoints;
    
        unsigned short  projWidth, projHeight;
    
        float   lbf[3], rtb[3];
        unsigned int    *iX, *iY;
    
        float           *xyz;
        float           *rgb;
        
        bool            allocatedFiltered;
        unsigned int    nPointsFiltered;
        float           *xyzFiltered;
        float           *rgbFiltered;
        unsigned int    *iXFiltered, *iYFiltered;
        
    
};