
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <mqtt/client.h>
#include <sstream>

#define ADDRESS     "ssl://abr0s9fb0umjr-ats.iot.us-east-2.amazonaws.com:8883"
#define CLIENTID    "new-rpi"
#define TOPIC       "road/damage"
#define QOS         1

#define CA_PATH     "/home/root/main_app/root-ca.pem"
#define CERT_PATH   "/home/root/main_app/certificate.pem.crt"
#define KEY_PATH    "/home/root/main_app/private.pem.key"

#define AI_FILE     "/home/root/main_app/ai.txt"
#define GPS_FILE    "/home/root/main_app/gps.txt"
#define BACKUP_FILE "/home/root/main_app/unsent.txt"

// Utility: Check file exists and is not empty
bool isNotEmpty(const std::string& filename) {
    std::ifstream file(filename);
    return file && file.peek() != std::ifstream::traits_type::eof();
}

// Utility: Store data locally in case of failure
void storeLocally(const std::string& payload) {
    std::ofstream out(BACKUP_FILE, std::ios::app);
    out << "\n"<<payload ;
    std::cout << "Stored locally: " << payload << std::endl;
}

// Utility: Read entire file into a string
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Utility: Clear file content
void clearFile(const std::string& filepath) {
    std::ofstream ofs(filepath, std::ios::trunc);
}

int main() {
   
        try {
            mqtt::client client(ADDRESS, CLIENTID);

            mqtt::ssl_options ssl_opts;
            ssl_opts.set_trust_store(CA_PATH);
            ssl_opts.set_key_store(CERT_PATH);
            ssl_opts.set_private_key(KEY_PATH);

            mqtt::connect_options conn_opts;
            conn_opts.set_ssl(ssl_opts);
            conn_opts.set_keep_alive_interval(std::chrono::seconds(20));
            conn_opts.set_clean_session(false);

            std::cout << "Connecting to AWS IoT Core..." << std::endl;
            client.connect(conn_opts);

            if (!client.is_connected()) {
                std::cerr << "Client failed to connect!" << std::endl;
                // throw mqtt::exception();
            }

            std::cout << "Connected to AWS IoT Core!" << std::endl;

            // --- Send current gps + ai if both are non-empty ---
            if (isNotEmpty(GPS_FILE) && isNotEmpty(AI_FILE)) {
                std::string gps_data = readFile(GPS_FILE);
                std::string ai_data = readFile(AI_FILE);
                std::string combined_payload = ai_data + gps_data;

                mqtt::message_ptr msg = mqtt::make_message(TOPIC, combined_payload, QOS, false);
                client.publish(msg);
                std::cout << "Published current data.\n";

               
            } else {
                std::cout << "One or both files are empty. Skipping current data.\n";
            }

           
            if (isNotEmpty(BACKUP_FILE)) {
    std::string all_unsent_data = readFile(BACKUP_FILE);

    if (!all_unsent_data.empty()) {
        mqtt::message_ptr backup_msg = mqtt::make_message(TOPIC, all_unsent_data, QOS, false);
        client.publish(backup_msg);
        std::cout << "Published all unsent data in one message.\n";
    }

    clearFile(BACKUP_FILE);
}


            client.disconnect();
            std::cout << "Disconnected from AWS IoT Core.\n";

        } catch (const mqtt::exception& exc) {
            std::cerr << "MQTT Exception: " << exc.what() << std::endl;

            // Store current data locally if valid
            if (isNotEmpty(GPS_FILE) && isNotEmpty(AI_FILE)) {
                std::string gps_data = readFile(GPS_FILE);
                std::string ai_data = readFile(AI_FILE);
                std::string combined_payload = ai_data + gps_data;
                storeLocally(combined_payload);

               
            }
        }

     

    return 0;
}
