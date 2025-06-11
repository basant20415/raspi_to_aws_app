
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <mqtt/client.h>
#include <sstream>

#define ADDRESS     "ssl://abr0s9fb0umjr-ats.iot.us-east-2.amazonaws.com:8883"
#define CLIENTID      "new-rpi"
#define TOPIC       "road/damage"
#define QOS         1
#define TIMEOUT     std::chrono::seconds(10)

// AWS IoT Core Credentials
#define CA_PATH     "./MQTT_APP-main/root-ca.pem"
#define CERT_PATH   "./MQTT_APP-main/certificate.pem.crt"
#define KEY_PATH    "./MQTT_APP-main/private.pem.key"
// Function to check for "gps_file" not empty
bool checkForExist(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

}

    int main(){
    try {
        // Create MQTT client
        mqtt::client client(ADDRESS, CLIENTID);

        // Configure SSL options
        mqtt::ssl_options ssl_opts;
        ssl_opts.set_trust_store(CA_PATH);
        ssl_opts.set_key_store(CERT_PATH);
        ssl_opts.set_private_key(KEY_PATH);

        // Configure connection options
        mqtt::connect_options conn_opts;
        conn_opts.set_ssl(ssl_opts);
        conn_opts.set_keep_alive_interval(std::chrono::seconds(20));
        conn_opts.set_clean_session(false);

        // Connect to AWS IoT Core
        std::cout << "Connecting to AWS IoT Core..." << std::endl;
        client.connect(conn_opts);
        //
        if (!client.is_connected()) {
    std::cerr << "Client failed to connect!" << std::endl;
    return 1;
    }
        std::cout << "Connected to AWS IoT Core!" << std::endl;

    
            if (checkForExist("./MQTT_APP-main/gps.txt")&&checkForExist("./MQTT_APP-main/ai.txt")) {
                std::cout << "Detected! Sending alert to AWS IoT Core..." << std::endl;

                   std::ifstream file1("./MQTT_APP-main/ai.txt");
std::ifstream file2("./MQTT_APP-main/gps.txt");

if (!file1 || !file2) {
    std::cerr << "Failed to open one or both files.\n";
    return 1;
}

std::string ai_data((std::istreambuf_iterator<char>(file1)), std::istreambuf_iterator<char>());
std::string gps_data((std::istreambuf_iterator<char>(file2)), std::istreambuf_iterator<char>());

// Construct plain text message (for example: "AI: ... GPS: ...")
std::ostringstream oss;
oss << ai_data << gps_data;

std::string combined_payload = oss.str();

// Publish once
mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, combined_payload, QOS, false);
client.publish(pubmsg);

std::cout << "Plain text message published to topic: " << TOPIC << std::endl;
                //  break;
            } else {
                std::cout << "No Location found." << std::endl;
            }
            // break;
            // std::this_thread::sleep_for(std::chrono::seconds(2));
           

        // Disconnect
        client.disconnect();
        std::cout << "Disconnected from AWS IoT Core." << std::endl;


    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT Exception: " << exc.what() << std::endl;
        return 1;
    }
    }
   
