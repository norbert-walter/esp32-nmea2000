#ifdef BOARD_OBP60S3

#include "Pagedata.h"
#include "OBP60Extensions.h"

const int SixValues_x1 = 5;
const int SixValues_DeltaX = 200;

const int SixValues_y1 = 23;
const int SixValues_DeltaY = 83;

const int HowManyValues = 6;

class PageSixValues : public Page
{
bool keylock = false;               // Keylock
int16_t lp = 80;                    // Pointer length

public:
    PageSixValues(CommonData &common){
        common.logger->logDebug(GwLog::LOG,"Show PageSixValues");
    }

    // Key functions
    virtual int handleKey(int key){
        // Keylock function
        if(key == 11){              // Code for keylock
            keylock = !keylock;     // Toggle keylock
            return 0;               // Commit the key
        }
        return key;
    }

  virtual void displayPage(CommonData &commonData, PageData &pageData){
        GwConfigHandler *config = commonData.config;
        GwLog *logger=commonData.logger;

        // Old values for hold function
        static String OldDataText[HowManyValues] = {"", "", "", "", "", ""};
        static String OldDataUnits[HowManyValues] = {"", "", "", "", "", ""};

        // Get config data
        String lengthformat = config->getString(config->lengthFormat);
        // bool simulation = config->getBool(config->useSimuData);
        bool holdvalues = config->getBool(config->holdvalues);
        String flashLED = config->getString(config->flashLED);
        String backlightMode = config->getString(config->backlight);
        
        GwApi::BoatValue *bvalue;
        String DataName[HowManyValues];
        double DataValue[HowManyValues];
        bool DataValid[HowManyValues];
        String DataText[HowManyValues];
        String DataUnits[HowManyValues];
        String DataFormat[HowManyValues];

        for (int i = 0; i < HowManyValues; i++){
            bvalue = pageData.values[i];
            DataName[i] = xdrDelete(bvalue->getName());
            DataName[i] = DataName[i].substring(0, 6);                  // String length limit for value name
            DataValue[i] = bvalue->value;                 // Value as double in SI unit
            DataValid[i] = bvalue->valid;
            DataText[i] = formatValue(bvalue, commonData).svalue;    // Formatted value as string including unit conversion and switching decimal places
            DataUnits[i] = formatValue(bvalue, commonData).unit;   
            DataFormat[i] = bvalue->getFormat();     // Unit of value
        }

        // Optical warning by limit violation (unused)
        if(String(flashLED) == "Limit Violation"){
            setBlinkingLED(false);
            setFlashLED(false); 
        }

        if (bvalue == NULL) return;
       
        // Draw page
        //***********************************************************

        // Set display in partial refresh mode
        getdisplay().setPartialWindow(0, 0, getdisplay().width(), getdisplay().height()); // Set partial update
        getdisplay().setTextColor(commonData.fgcolor);

        for (int i = 0; i < ( HowManyValues / 2 ); i++){
   // Horizontal line 3 pix
            getdisplay().fillRect(0, SixValues_y1+i*SixValues_DeltaY, 400, 3, commonData.fgcolor);

            for (int j = 0; j < 2; j++){
                int ValueIndex = i * 2 + j;
                int x0 = SixValues_x1 + j * SixValues_DeltaX;
                int y0 = SixValues_y1 + i * SixValues_DeltaY;
                LOG_DEBUG(GwLog::LOG,"Drawing at PageSixValue: %d %s %f",  ValueIndex,  DataName[ValueIndex], DataValue[ValueIndex] );

       // Show name
                getdisplay().setFont(&Ubuntu_Bold12pt7b);
                getdisplay().setCursor(x0, y0+25);
                getdisplay().print(DataName[ValueIndex]);                           // Page name

        // Show unit
                getdisplay().setFont(&Ubuntu_Bold8pt7b);
                getdisplay().setCursor(x0, y0+72);
                if(holdvalues == false){
                    getdisplay().print(DataUnits[ValueIndex]);                       // Unit
                    }
                else{
                    getdisplay().print(OldDataUnits[ValueIndex]);
                    }

        // Switch font if format for any values
                if(DataFormat[ValueIndex] == "formatLatitude" || DataFormat[ValueIndex] == "formatLongitude"){
                    getdisplay().setFont(&Ubuntu_Bold12pt7b);
                    getdisplay().setCursor(x0+10, y0+60);
                    }
                else if(DataFormat[ValueIndex] == "formatTime" || DataFormat[ValueIndex] == "formatDate"){
                    getdisplay().setFont(&Ubuntu_Bold16pt7b);
                    getdisplay().setCursor(x0+20,y0+55);
                    }           
                else if(DataFormat[ValueIndex] == "formatXdr:P:P"){
                    getdisplay().setFont(&DSEG7Classic_BoldItalic26pt7b);
                    getdisplay().setCursor(x0+25, y0+70);
                 }
                else{
                    getdisplay().setFont(&DSEG7Classic_BoldItalic26pt7b);
                    if ( DataText[ValueIndex][0] == '-' )
                        getdisplay().setCursor(x0+25, y0+70);
                    else
                        getdisplay().setCursor(x0+65, y0+70);
                    }

        // Show bus data
                if(holdvalues == false){
                    getdisplay().print(DataText[ValueIndex]);                                     // Real value as formated string
                    }
                else{
                    getdisplay().print(OldDataText[ValueIndex]);                                  // Old value as formated string
                    }
                if(DataValid[ValueIndex] == true){
                    OldDataText[ValueIndex] = DataText[ValueIndex];                                       // Save the old value
                    OldDataUnits[ValueIndex] = DataUnits[ValueIndex];                                           // Save the old unit
                    }
            }
               // Vertical line 3 pix
        getdisplay().fillRect(SixValues_x1+SixValues_DeltaX-8, SixValues_y1+i*SixValues_DeltaY, 3, SixValues_DeltaY, commonData.fgcolor);
        }
        getdisplay().fillRect(0, SixValues_y1+3*SixValues_DeltaY, 400, 3, commonData.fgcolor);


        // ############### Key Layout ################

        // Key Layout
        getdisplay().setFont(&Ubuntu_Bold8pt7b);
        if(keylock == false){
            getdisplay().setCursor(130, 290);
            getdisplay().print("[  <<<<  " + String(commonData.data.actpage) + "/" + String(commonData.data.maxpage) + "  >>>>  ]");
            if(String(backlightMode) == "Control by Key"){                  // Key for illumination
                getdisplay().setCursor(343, 290);
                getdisplay().print("[ILUM]");
            }
        }
        else{
            getdisplay().setCursor(130, 290);
            getdisplay().print(" [    Keylock active    ]");
        }

        // Update display
        getdisplay().nextPage();    // Partial update (fast)

    };

};

static Page *createPage(CommonData &common){
    return new PageSixValues(common);
}
/**
 * with the code below we make this page known to the PageTask
 * we give it a type (name) that can be selected in the config
 * we define which function is to be called
 * and we provide the number of user parameters we expect (0 here)
 * and will will provide the names of the fixed values we need
 */
PageDescription registerPageSixValues(
    "SixValues",         // Page name
    createPage,         // Action
    6,                  // Number of bus values depends on selection in Web configuration; was zero
    //{"AWA", "AWS", "COG", "SOG", "TWD", "TWS"},    // Bus values we need in the page, modified for WindRose2
    true                // Show display header on/off
);

#endif
