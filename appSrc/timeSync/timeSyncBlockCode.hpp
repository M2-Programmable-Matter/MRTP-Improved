#ifndef TimeSyncBlockCode_H_
#define TimeSyncBlockCode_H_

#include "robots/blinkyBlocks/blinkyBlocksWorld.h"
#include "robots/blinkyBlocks/blinkyBlocksBlockCode.h"
#include <set>
#include "clock/qclock.h"
static const int GO_MSG_ID = 1000;
// static const int Back_MSG_ID_yes  = 1002;
static const int Back_MSG_ID = 1001;
static const int BTIME_MSG_ID = 1002;
static const int TEST_MSG_ID = 1003;
static const int SYNC_MSG_ID = 1011;

static Time maxTime=0;
static Time minTime=0;
//static int maxd=0;
//static DNoiseQClock* createXMEGA_RTC_OSC1K_CRC(ruint seed);
using namespace BlinkyBlocks;

struct sds {
    Time ltime;
    Time gtime;
};

struct messag {
    Time t;
    Time d;
};

class TimeSyncBlockCode : public BlinkyBlocksBlockCode {
private:

    int distance;
    BlinkyBlocksBlock *module;
    P2PNetworkInterface* parent = nullptr;
    set<P2PNetworkInterface*> children;
    vector<sds> W;
    int nbWaitedAnwsers = 0;
    Time local=0;
    Time global=0;
    Time T=0;
    Time Tparent=0;
    Time Tprime=0;
    Time Deltaparent=0;
    Time Delta=0;
    Time Deltaprime=0;
    float a = 1.0;
    float b = 0.0 ;
    set<P2PNetworkInterface*>::iterator it ;
    

public :
    TimeSyncBlockCode(BlinkyBlocksBlock *host);
    ~TimeSyncBlockCode() {};

    /**
     * This function is called on startup of the blockCode, it can be used to perform initial
     *  configuration of the host or this instance of the program.
     * @note this can be thought of as the main function of the module
     **/
    void startup() override;
    /**
     * @brief Handler for all events received by the host block
     * @param pev pointer to the received event
     */
    void processLocalEvent(EventPtr pev) override;
    
    /**
     * @brief Callback function executed whenever the module finishes a motion
     */
    void onMotionEnd() override;

    /**
     * @brief Sample message handler for this instance of the blockcode
     * @param _msg Pointer to the message received by the module, requires casting
     * @param sender Connector of the module that has received the message and that is connected to the sender */
    void handleGoMessage(std::shared_ptr<Message> _msg, P2PNetworkInterface *sender);

    
    void handleBackMessage(std::shared_ptr<Message> _msg, P2PNetworkInterface *sender);
    void handleTestMessage(std::shared_ptr<Message> _msg, P2PNetworkInterface *sender);
    void handleSyncMessage(std::shared_ptr<Message> _msg, P2PNetworkInterface *sender);
    void handleBtimeMessage(std::shared_ptr<Message> _msg, P2PNetworkInterface *sender);
    //void handleBackMessageyes(std::shared_ptr<Message> _msg, P2PNetworkInterface *sender);
    //void  TimeSyncBlockCode::Computelinearregression();
    /// Advanced blockcode handlers below

    /**
     * @brief Provides the user with a pointer to the configuration file parser, which can be used to read additional user information from it.
     * @param config : pointer to the TiXmlDocument representing the configuration file, all information related to VisibleSim's core have already been parsed
     *
     * Called from BuildingBlock constructor, only once.
     */
    void parseUserElements(TiXmlDocument *config) override {}

    /**
     * @brief Provides the user with a pointer to the configuration file parser, which can be used to read additional user information from each block config. Has to be overriden in the child class.
     * @param config : pointer to the TiXmlElement representing the block configuration file, all information related to concerned block have already been parsed
     *
     */
    void parseUserBlockElements(TiXmlElement *config) override {}

    /**
     * User-implemented debug function that gets called when a module is selected in the GUI
     */
    void onBlockSelected() override;

    /**
     * User-implemented debug function that gets called when a VS_ASSERT is triggered
     * @note call is made from utils::assert_handler()
     */
    void onAssertTriggered() override;

    /**
     * User-implemented keyboard handler function that gets called when
     *  a key press event could not be caught by openglViewer
     * @param c key that was pressed (see openglViewer.cpp)
     * @param x location of the pointer on the x axis
     * @param y location of the pointer on the y axis
     * @note call is made from GlutContext::keyboardFunc (openglViewer.h)
     */
    void onUserKeyPressed(unsigned char c, int x, int y) override {}

    void  Computelinearregression();
    Time getGlobalTime();
     Time getTprime();
    /**
     * Call by world during GL drawing phase, can be used by a user
     *  to draw custom Gl content into the simulated world
     * @note call is made from World::GlDraw
     */
    void onGlDraw() override {}

    /**
     * @brief This function is called when a module is tapped by the user. Prints a message to the console by default.
     Can be overloaded in the user blockCode
     * @param face face that has been tapped */
    void onTap(int face) override {}


    /**
     * User-implemented keyboard handler function that gets called when
     *  a key press event could not be caught by openglViewer
     * @note call is made from GlutContext::keyboardFunc (openglViewer.h)
     */
    bool parseUserCommandLineArgument(int& argc, char **argv[]) override;

    /**
     * Called by openglviewer during interface drawing phase, can be used by a user
     *  to draw a custom Gl string onto the bottom-left corner of the GUI
     * @note call is made from OpenGlViewer::drawFunc
     * @return a string (can be multi-line with `\n`) to display on the GUI
     */
    string onInterfaceDraw() override;

/*****************************************************************************/
/** needed to associate code to module                                      **/
    static BlockCode *buildNewBlockCode(BuildingBlock *host) {
        return (new TimeSyncBlockCode((BlinkyBlocksBlock*)host));
    };
/*****************************************************************************/
};

#endif /* TimeSyncBlockCode_H_ */
