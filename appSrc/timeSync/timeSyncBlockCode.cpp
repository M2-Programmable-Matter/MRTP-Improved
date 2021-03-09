#include "timeSyncBlockCode.hpp"
#include "clock/qclock.h"
using namespace BlinkyBlocks;

TimeSyncBlockCode::TimeSyncBlockCode(BlinkyBlocksBlock *host) : BlinkyBlocksBlockCode(host) {
    // @warning Do not remove block below, as a blockcode with a NULL host might be created
    //  for command line parsing
    if (not host) return;

    // Registers a callback (handleSampleMessage) to the message of type SAMPLE_MSG_ID
    addMessageEventFunc2(GO_MSG_ID,
                         std::bind(&TimeSyncBlockCode::handleGoMessage, this,
                                   std::placeholders::_1, std::placeholders::_2));
    addMessageEventFunc2(SYNC_MSG_ID,
                         std::bind(&TimeSyncBlockCode::handleSyncMessage, this,
                                   std::placeholders::_1, std::placeholders::_2));

    addMessageEventFunc2(BTIME_MSG_ID,
                         std::bind(&TimeSyncBlockCode::handleBtimeMessage, this,
                                   std::placeholders::_1, std::placeholders::_2));
    addMessageEventFunc2(Back_MSG_ID,
                         std::bind(&TimeSyncBlockCode::handleBackMessage, this,
                                   std::placeholders::_1, std::placeholders::_2));
    addMessageEventFunc2(TEST_MSG_ID,
                         std::bind(&TimeSyncBlockCode::handleTestMessage, this,
                                   std::placeholders::_1, std::placeholders::_2));

    // Set the module pointer
    module = static_cast<BlinkyBlocksBlock*>(hostBlock);
    module->setClock(DNoiseQClock::createXMEGA_RTC_OSC1K_CRC(module->blockId));
 
    
}

void TimeSyncBlockCode::startup() {
    // setClock(DNoiseQClock::createXMEGA_RTC_OSC1K_CRC(rand() % 100));
    console << "start";
   
    // Sample distance coloring algorithm below
    if (module->blockId == 1) { // Master ID is 1
        module->setColor(BLACK);
        // cl->getTime(module->getLocalTime());
        module->getLocalTime();
        distance = 0;
        nbWaitedAnwsers = 0;
        for(int i=0; i<module->getNbInterfaces(); i++) {
            if(module->getInterface(i)->isConnected()){
                sendMessage("Go Msg", new MessageOf<int>(GO_MSG_ID, distance),
                    module->getInterface(i), 100, 200);
                nbWaitedAnwsers++;
                
            }
        }
    } else {
        distance = -1; // Unknown distance
        hostBlock->setColor(LIGHTGREY);
    }

    // Additional initialization and algorithm start below
    // ...
}

void TimeSyncBlockCode::handleGoMessage(std::shared_ptr<Message> _msg,
                                               P2PNetworkInterface* sender) {
    MessageOf<int>* msg = static_cast<MessageOf<int>*>(_msg.get());
    
    int d = *msg->getData()+1;
   // console << " received d =" << d << " from " << sender->getConnectedBlockId() << "\n";
   // console<< module->getLocalTime() << "\n";
 
    if (distance == -1 || distance > d) {
        // if already has parent (parent == nullptr)
        // then send back false
     //   console << " updated distance = " << d << "\n";
        distance = d;
        parent = sender;
        console << "parent: " << parent->getConnectedBlockId() << "\n";
        module->setColor(Colors[distance % NB_COLORS]);
        // Broadcast to all neighbors but ignore sender
        nbWaitedAnwsers = 0;
        for(int i=0; i<module->getNbInterfaces(); i++) {
            if(module->getInterface(i)->isConnected() && module->getInterface(i) != sender){
                sendMessage("Go Msg", new MessageOf<int>(GO_MSG_ID, distance),
                    module->getInterface(i), 100, 200);
                nbWaitedAnwsers++;
            }
        }
        if(nbWaitedAnwsers == 0) {
            // send back true
            if(module->getNbNeighbors()==1){
                local = module->getLocalTime();
              //  global=module->getSimulationTime(module->getLocalTime());
              //  offset = global - local ;
          //      console<<"local "<<local<<"\n";
            //    console<<"global "<<global<<"\n";
              //  console<<"offser "<<offset<<"\n";
                sendMessage("Back Time Msg", new MessageOf<Time>(BTIME_MSG_ID,local),sender, 100, 200);

            }
            vector<int> res;
            //vector<int> id;
            //id.push_back(Back_MSG_ID_yes);
            res.push_back(1);
            res.push_back(distance);
       //     console<<sender<<"\n";
       //     console<<"hass"<<"\n";
            sendMessage("Back Msg", new MessageOf<vector<int>>(Back_MSG_ID, res),
                    sender, 100, 200);
        }
    } else {
        vector<int> res;
            //vector<int> id;
            //id.push_back(Back_MSG_ID_yes);
            res.push_back(0);
            res.push_back(distance);
            sendMessage("Back Msg", new MessageOf<vector<int>>(Back_MSG_ID, res),
                    sender, 100, 200);
        // send back false
    }
    console<<"nbwaited msg= "<<nbWaitedAnwsers<<"\n";
}

void TimeSyncBlockCode::handleBackMessage(std::shared_ptr<Message> _msg,
                                               P2PNetworkInterface* sender) {
    MessageOf<vector<int>>* msg = static_cast<MessageOf<vector<int>>*>(_msg.get());
    console << " received back from =" << sender->getConnectedBlockId()<< "\n";
    vector<int> r = *msg->getData() ;
    //cout<<"yes msg or no ? "<<r[0]<<" distance "<<r[1]<<endl;
    //console << " yes or no? " << r[0] << "\n";
    //console << distance+1 << "and r[1] "<<r[1] << "\n";
    nbWaitedAnwsers--;
                

    if(r[1]==distance+1 || r[1]+1==distance ){
        if(r[0]==1 && r[1]==distance+1){
            children.insert(sender);
          //  if(offset2>offset){offset=offset2;}
        }
        if(nbWaitedAnwsers == 0) {
            if(parent != nullptr){
                
                sendMessage("Back Msg", new MessageOf<vector<int>>(Back_MSG_ID, {1,distance}),
                        parent, 100, 200);
               local = module->getLocalTime();
                //global=module->getSimulationTime(module->getLocalTime());
                //if(offset2==0){
                //offset = global - local ; }
            //    if(offset2>offset){offset=offset2;}
                sendMessage("Back Time Msg", new MessageOf<Time>(BTIME_MSG_ID,local),parent, 100, 200);                
            } else {
                for(auto c: children){
                    sendMessage("Test Msg", new Message(TEST_MSG_ID),
                            c, 100, 200);
                }
            }
        }
    }
console<<"nbwaited msg= "<<nbWaitedAnwsers<<"\n";

}


void TimeSyncBlockCode::handleTestMessage(std::shared_ptr<Message> _msg,
                                               P2PNetworkInterface* sender) {
 //   console << "Recieved Test Message from: " << sender->getConnectedBlockId() << "\n";
    module->setColor(children.size());
 //   console << "children: " << "\n";
    for(auto c: children){
        console << c->getConnectedBlockId() << " ";
    }
        console << "\n";

    for(auto c: children){
        sendMessage("Test Msg", new Message(TEST_MSG_ID),
                c, 100, 200);
    }
}
void TimeSyncBlockCode::handleBtimeMessage(std::shared_ptr<Message> _msg,
                                               P2PNetworkInterface* sender) {
            MessageOf<Time>* msg = static_cast<MessageOf<Time>*>(_msg.get());
            Time t = *msg->getData();
           // t[0]=t[0]+ 0.1;//d0 or dt??
            local = module->getLocalTime();
            console<<"my local time="<< module->getLocalTime() <<"\n";
            //if(local<t){
           //     local=t;
           // }
            //global=module->getSimulationTime(module->getLocalTime());
            //offset = global - local;
            //console<<"received old global "<< t[0]<<"\n";
            //offset2=t[0]-local;
            //console<<"old offset "<< offset <<"\n";
            //if(offset2>offset){offset=offset2;}
            //console<<"new offset : "<< offset2 <<"\n";
            //console<<"regulated offset : "<< offset <<"\n";
            
                if(module->blockId ==1 && nbWaitedAnwsers==0){
                    
                    console<<children.size();
                    //it=children.begin();
                    //for(int i = 0 ; i<children.size() && it != children.end();i++){
                    console<<"\n"<<local<<"\n";
                    
                    for(auto c: children){
                        sendMessage("Sync Msg", new MessageOf<pair<Time,Time>>(SYNC_MSG_ID,make_pair(local,1)),
                        c, 100, 200);
                        //console<<(*it)<<"\n";
                        //it++;
                        
                    }
                    
                    getScheduler()->schedule( new InterruptionEvent(getScheduler()->now() + 5000000,this->module, 1010));
                    //schedule 5 sec
                }   
        }


Time TimeSyncBlockCode::getGlobalTime() {
    return round(a*module->getLocalTime() + b);
    // TimeSyncBlockCode *root = (TimeSyncBlockCode*) BaseSimulator::getWorld()->getBlockById(1)->blockCode;
    // root->getGlobalTime();
} 


void TimeSyncBlockCode::handleSyncMessage(std::shared_ptr<Message> _msg,
                                               P2PNetworkInterface* sender) {
        //MessageOf<vector<Time>>* msg = static_cast<MessageOf<vector<Time>>*>(_msg.get());                                           
        MessageOf<pair<Time,Time>>* msg = static_cast<MessageOf<pair<Time,Time>>*>(_msg.get());   
        pair<Time,Time> msgData = *msg->getData();                                         
        console<<"receieved sync from: "<<sender->getConnectedBlockId()<<"\n";
        //global=t;
        //global=*msg->getData();
        //console<<"recieved is = "<<*msg->getData()<<"\n"<<"and the local= "<<module->getLocalTime()<<"\n";
        sds sd;
        sd.gtime=msgData.first;
        sd.ltime=module->getLocalTime();
        //vector<Time> synchpts;
        //synchpts.push_back(t[0]);
        //synchpts.push_back(local);
        if(W.size()>4){
            W.erase(W.begin());
        }
        W.push_back(sd);
       /* for(auto sd: W){
            console<<sd.ltime<<"\n";
            console << sd.ltime << "\n";
        } */
        if(W.size() > 1) {
            Computelinearregression();
        }
        Tparent=msgData.first;
        Deltaparent=msgData.second;
        
        T=getGlobalTime();
        Delta=(Tparent-T);
       // cout<<"loook hon ::  "<<T<<endl;
        Deltaprime=(pow(Deltaparent,2)*pow(Delta,2))/(pow(Deltaparent,2)+pow(Delta,2));
        Deltaprime=sqrt(Deltaprime);
        Tprime=((pow(Delta,2)*Tparent)/(pow(Deltaparent,2)+pow(Delta,2)))+((pow(Deltaparent,2)*T)/(pow(Deltaparent,2)+pow(Delta,2)));

        
        for(auto c: children){
            sendMessage("Sync Msg", new MessageOf<pair<Time,Time>>(SYNC_MSG_ID, make_pair(Tprime,Deltaprime)),
            c, 100, 200);
            //console<<c<<"\n";
            //console<<(*it)<<"\n";
            //it++;
            }
        //if(children.size()==0){
        //    console<<"I DON'T HAVE CHILDREN,I HAVE (MAYBE) MAXT"<<"\n";
        //    console<<"MY T IS = "<< getGlobalTime()<<"\n";
        //    if(getGlobalTime()>maxTime){
        //        console<<"DING DING DING"<<"\n";
        //        maxTime=getGlobalTime();
        //    }
       // }
        //computing the linear regression ...i.
    }



/* void TimeSyncBlockCode::processLocalEvent(EventPtr pev) {
    std::shared_ptr<Message> message;
    stringstream info;

    // Do not remove line below
    BlinkyBlocksBlockCode::processLocalEvent(pev);

    switch (pev->eventType) {
        case EVENT_ADD_NEIGHBOR: {
            // Do something when a neighbor is added to an interface of the module
            break;
        }

        case EVENT_REMOVE_NEIGHBOR: {
            // Do something when a neighbor is removed from an interface of the module
            break;
        }
    }
} */

Time TimeSyncBlockCode::getTprime() {
        T=getGlobalTime();
        Delta=(Tparent-T);
       // cout<<"loook hon ::  "<<T<<endl;
        Deltaprime=(pow(Deltaparent,2)*pow(Delta,2))/(pow(Deltaparent,2)+pow(Delta,2));
        Deltaprime=sqrt(Deltaprime);
        Tprime=((pow(Delta,2)*Tparent)/(pow(Deltaparent,2)+pow(Delta,2)))+((pow(Deltaparent,2)*T)/(pow(Deltaparent,2)+pow(Delta,2)));
        return round(Tprime);
} 


void TimeSyncBlockCode::processLocalEvent(EventPtr pev) {

    /// Other code 
     std::shared_ptr<Message> message;
    stringstream info;

    // Do not remove line below
    BlinkyBlocksBlockCode::processLocalEvent(pev);

    switch (pev->eventType) {

            /// .... ALL THE OTHER EVENT STUFF
            
        case EVENT_INTERRUPTION: {
            std::shared_ptr<InterruptionEvent> itev =
                std::static_pointer_cast<InterruptionEvent>(pev);

            switch(itev->mode) {
                case 1010: {
                        minTime = module->getLocalTime();

                        int nbModules = BaseSimulator::getWorld()->getNbBlocks();
                        for(int i=1 ;i<=nbModules ; i++){
                        TimeSyncBlockCode *x = (TimeSyncBlockCode*) BaseSimulator::getWorld()->getBlockById(i)->blockCode;
                        if(x->getTprime()>maxTime && x->Tprime<minTime+50000000){
                            maxTime=x->getGlobalTime();
                            }
                        }
                        //cout<<"MINT IS    =   "<<minTime<<"   and MAXT is =  "<<maxTime<<endl; 
                       // if(maxTime>5000000){
                        cout<<minTime<<";"<<maxTime<<endl;
                       // }
                        for(auto c: children){
                        sendMessage("Sync Msg", new MessageOf<pair<Time,Time>>(SYNC_MSG_ID,make_pair(this->module->getLocalTime(),1)),
                        c, 100, 200);
                    }
                    maxTime=0;
                             std::shared_ptr<Message> message;
stringstream info;

    // Do not remove line below
    BlinkyBlocksBlockCode::processLocalEvent(pev);
                        getScheduler()->schedule( new InterruptionEvent(getScheduler()->now() + 5000000,this->module, 1010));
                } 
            }
        }
    }
}


void  TimeSyncBlockCode::Computelinearregression(){
    float x_mean=0,y_mean=0;
    float SS_xx=0,SS_xy=0;
    console<<"W: ";
    
    for(auto sd: W){
        x_mean+=sd.ltime;
        y_mean+=sd.gtime;
        console << sd.ltime << "  " <<sd.gtime <<"\n";
    }
    x_mean/= W.size();
    y_mean/=W.size();
    for(auto sd: W){
        SS_xy+=((float)sd.ltime-x_mean)*((float)sd.gtime-y_mean);
        SS_xx+=pow((float)sd.ltime-x_mean,2);
    }
    a=SS_xy/SS_xx;
    console<<"ss_xx= "<< SS_xx <<"\n";
    b=y_mean-a * x_mean;
    console<<"a = "<<a<<"\n";
    console<<"b = "<<b<<"\n";
}

void TimeSyncBlockCode::onMotionEnd() {
    console << " has reached its destination" << "\n";

    // do stuff
    // ...
}

/// ADVANCED BLOCKCODE FUNCTIONS BELOW

void TimeSyncBlockCode::onBlockSelected() {
    // Debug stuff:
    console<<"lol"<<"\n";
    
    cerr << endl << "--- PRINT MODULE " << *module << "---" << endl;
    cerr << "parent: " << parent->getConnectedBlockId() << endl;
}

void TimeSyncBlockCode::onAssertTriggered() {
    console << " has triggered an assert" << "\n";

    // Print debugging some info if needed below
    // ...
}

bool TimeSyncBlockCode::parseUserCommandLineArgument(int &argc, char **argv[]) {
    /* Reading the command line */
    if ((argc > 0) && ((*argv)[0][0] == '-')) {
        switch((*argv)[0][1]) {

            // Single character example: -b
            case 'b':   {
                cout << "-b option provided" << endl;
                return true;
            } break;

            // Composite argument example: --foo 13
            case '-': {
                string varg = string((*argv)[0] + 2); // argv[0] without "--"
                if (varg == string("foo")) { //
                    int fooArg;
                    try {
                        fooArg = stoi((*argv)[1]);
                        argc--;
                        (*argv)++;
                    } catch(std::logic_error&) {
                        stringstream err;
                        err << "foo must be an integer. Found foo = " << argv[1] << endl;
                        throw CLIParsingError(err.str());
                    }

                    cout << "--foo option provided with value: " << fooArg << endl;
                } else return false;

                return true;
            }

            default: cerr << "Unrecognized command line argument: " << (*argv)[0] << endl;
        }
    }

    return false;
}

string TimeSyncBlockCode::onInterfaceDraw() {
    stringstream trace;
    trace << "Some value " << 123;
    return trace.str();
}

