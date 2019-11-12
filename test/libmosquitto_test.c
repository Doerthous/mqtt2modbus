#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <cjson/cJSON.h>

#define HOST "112.124.25.77"
#define PORT 1883
#define KEEP_ALIVE 60
#define TOPIC "/test/topic"
#define READ_TOPIC "/read"
#define WRITE_TOPIC "/write"

static char *host = HOST;
static int port = PORT;
static int keep_alive = KEEP_ALIVE;
static char *read_topic = READ_TOPIC;
static char *write_topic = WRITE_TOPIC;

static bool session = true;

struct mosquitto *rx = NULL;


// callbacks
static void subscribe_on_connect(struct mosquitto *mosq, void *data, int result)
{
    if (!result) {
        // subscribe when connect successed.
        mosquitto_subscribe(mosq, NULL, (char *)data, 1); // data is mqtt topic
    }
    else {
        perror("connection refused.\n");
    }
}

static void message_callback(struct mosquitto *mosq, void *data,
    const struct mosquitto_message *msg)
{
    if (msg->payloadlen) {
        //printf("%s %s\n", msg->topic, (const char *)msg->payload);
        
        cJSON *json = cJSON_Parse((const char *)msg->payload);
  
        if (json == NULL) {
            perror("json parse failed.\n");
            return;
        }

        char *jstr = cJSON_Print(json);

        printf("%s\n", jstr);
        
        cJSON_Delete(json);
        free(jstr);
    }
    else {
        printf("%s (null)\n", msg->topic);
    }
}

static void read_topic_cb(struct mosquitto *mosq, void *data,
    const struct mosquitto_message *msg)
{
    
}

static void write_topic_cb(struct mosquitto *mosq, void *data,
    const struct mosquitto_message *msg)
{
    
}


// configuration
static int parse_config(const char *conf)
{
    host = HOST;
    port = PORT;
    keep_alive = KEEP_ALIVE;
    read_topic = READ_TOPIC;
    write_topic = WRITE_TOPIC;

    return 1;
}



int main(void)
{
    mosquitto_lib_init(); // always return success.
    
    // read configure file
    // configure...
    if (!parse_config("./mqtt2modbus.conf")) {
        perror("configuration file not found in current directory.\n");
        return 1; // ./mqtt2modbus.conf not found
    }
    
    // create all mqtt object and connect them to server
    if ((rx = mosquitto_new(NULL, true, read_topic)) == NULL) {
        perror("create client failed.\n");
        return 2; // create client failed
    }

    mosquitto_connect_callback_set(rx, subscribe_on_connect);
    mosquitto_message_callback_set(rx, message_callback);

    if (mosquitto_connect(rx, host, port, keep_alive)) {
        perror("unable to connect.\n");
        return 3; // connect failed
    }

    mosquitto_loop_forever(rx, -1, 1);

    // clean up resources
    mosquitto_destroy(rx);
    mosquitto_lib_cleanup();    

    return 0; // never reach here
}
