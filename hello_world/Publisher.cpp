#include <iostream>
#include <ace/Log_Msg.h>
#include <dds/DdsDcpsInfrastructureC.h> // client generatead header defining commin interface (listener, entity...)
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include "MyMessengerTypeSupportImpl.h"

DDS::DomainId_t HELLO_WORLD_DOMAIN_ID       = 1000;
const char* MY_MESSAGE_TYPE     = "My Message Type";
const char* MY_MESSAGE_TOPIC    = "My Message Topic"; 


int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {
    std::cout << "Publisher\n";
    try {

        // Initialize DomainParticipantFactory
        DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);

        // Create DomainParticipant
        DDS::DomainParticipant_var participant = dpf->create_participant(HELLO_WORLD_DOMAIN_ID,
                                                                        PARTICIPANT_QOS_DEFAULT,
                                                                        DDS::DomainParticipantListener::_nil(),
                                                                        OpenDDS::DCPS::DEFAULT_STATUS_MASK); 

        if (CORBA::is_nil(participant.in())) {
            std::cerr << "create_participant failed\n";
            ACE_OS::exit(1);
        }  

        // Create publisher 
        DDS::Publisher_var publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                                                    DDS::PublisherListener::_nil(),
                                                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(publisher.in())) {
            std::cerr << "create_publisher failed\n";
            ACE_OS::exit(1);
        }

        // Register type suport "My Message Type"
        MyMessenger::MyMessageTypeSupport_var message_servant = new MyMessenger::MyMessageTypeSupportImpl();
        if (DDS::RETCODE_OK != message_servant->register_type(participant.in(),
                                                                MY_MESSAGE_TYPE)) {
            std::cerr << "register_type failed\n";
            ACE_OS::exit(1);
        }

        // After have data type, we create topic name "My Message Topic"
        DDS::Topic_var my_message_topic = participant->create_topic(MY_MESSAGE_TOPIC,   // topic name
                                                                    MY_MESSAGE_TYPE,    // data type binding
                                                                    TOPIC_QOS_DEFAULT,
                                                                    DDS::TopicListener::_nil(),
                                                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);    
        if (CORBA::is_nil(my_message_topic.in())) {
            std::cerr << "create_topic failed\n";
            ACE_OS::exit(1);
        }

        // After registered the topic, we create data writer for "My Message Topic"
        DDS::DataWriter_var my_message_base_dw = publisher->create_datawriter(my_message_topic.in (),
                                                                    DATAWRITER_QOS_DEFAULT,
                                                                    DDS::DataWriterListener::_nil(),
                                                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);    

        if (CORBA::is_nil (my_message_base_dw.in ()))
        {
            std::cerr << "create_datawriter failed\n";
            ACE_OS::exit(1);
        }

        // After created base data writer, we "narrow" it to specific data writer
        MyMessenger::MyMessageDataWriter_var my_message_dw = MyMessenger::MyMessageDataWriter::_narrow(my_message_base_dw.in());

        // Wait until 

    } catch (const CORBA::Exception& e) {
        e._tao_print_exception("Exception caught in main():");
        return 1;
    }


    return 0;
}