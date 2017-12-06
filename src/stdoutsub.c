/*******************************************************************************
 * Copyright (c) 2012, 2016 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *    Ian Craggs - change delimiter option from char to string
 *    Al Stockdill-Mander - Version using the embedded C client
 *    Ian Craggs - update MQTTClient function names
 *******************************************************************************/

/*
 
 stdout subscriber
 
 compulsory parameters:
 
  topic to subscribe to
 
 defaulted parameters:
 
  --host localhost
  --port 1883
  --qos 2
  --delimiter \n
  --clientid stdout_subscriber

  --userid none
  --password none

 for example:

    stdoutsub topic/of/interest --host iot.eclipse.org

*/
#include <stdio.h>
#include <memory.h>
#include "MQTTClient.h"

#include <stdio.h>
#include <signal.h>

#include <sys/time.h>


volatile int toStop = 0;


void usage()
{
  printf("MQTT stdout subscriber\n");
  printf("Usage: stdoutsub topicname <options>, where options are:\n");
  printf("  --host <hostname> (default is localhost)\n");
  printf("  --port <port> (default is 1883)\n");
  printf("  --qos <qos> (default is 2)\n");
  printf("  --delimiter <delim> (default is \\n)\n");
  printf("  --clientid <clientid> (default is hostname+timestamp)\n");
  printf("  --username none\n");
  printf("  --password none\n");
  printf("  --showtopics <on or off> (default is on if the topic has a wildcard, else off)\n");
  exit(-1);
}


void cfinish(int sig)
{
  signal(SIGINT, NULL);
  toStop = 1;
}


struct opts_struct
{
  char* clientid;
  int nodelimiter;
  char* delimiter;
  enum QoS qos;
  char* username;
  char* password;
  char* host;
  int port;
  int showtopics;
} opts =
{
    (char*)"6296722", 0, (char*)"\n", QOS0, "86338", "rtos", (char*)"183.230.40.39", 6002, 1
};


void getopts(int argc, char** argv)
{
  int count = 2;

  while (count < argc)
  {
    if (strcmp(argv[count], "--qos") == 0)
    {
      if (++count < argc)
      {
        if (strcmp(argv[count], "0") == 0)
          opts.qos = QOS0;
        else if (strcmp(argv[count], "1") == 0)
          opts.qos = QOS1;
        else if (strcmp(argv[count], "2") == 0)
          opts.qos = QOS2;
        else
          usage();
      }
      else
        usage();
    }
    else if (strcmp(argv[count], "--host") == 0)
    {
      if (++count < argc)
        opts.host = argv[count];
      else
        usage();
    }
    else if (strcmp(argv[count], "--port") == 0)
    {
      if (++count < argc)
        opts.port = atoi(argv[count]);
      else
        usage();
    }
    else if (strcmp(argv[count], "--clientid") == 0)
    {
      if (++count < argc)
        opts.clientid = argv[count];
      else
        usage();
    }
    else if (strcmp(argv[count], "--username") == 0)
    {
      if (++count < argc)
        opts.username = argv[count];
      else
        usage();
    }
    else if (strcmp(argv[count], "--password") == 0)
    {
      if (++count < argc)
        opts.password = argv[count];
      else
        usage();
    }
    else if (strcmp(argv[count], "--delimiter") == 0)
    {
      if (++count < argc)
        opts.delimiter = argv[count];
      else
        opts.nodelimiter = 1;
    }
    else if (strcmp(argv[count], "--showtopics") == 0)
    {
      if (++count < argc)
      {
        if (strcmp(argv[count], "on") == 0)
          opts.showtopics = 1;
        else if (strcmp(argv[count], "off") == 0)
          opts.showtopics = 0;
        else
          usage();
      }
      else
        usage();
    }
    count++;
  }

}


void messageArrived(MessageData* md)
{
  MQTTMessage* message = md->message;

  printf("Get topic: \n");
  if (opts.showtopics)
    printf("%.*s\t", md->topicName->lenstring.len, md->topicName->lenstring.data);
  if (opts.nodelimiter)
    printf("%.*s", (int)message->payloadlen, (char*)message->payload);
  else
    printf("%.*s%s", (int)message->payloadlen, (char*)message->payload, opts.delimiter);
  //fflush(stdout);
}


static int iotMqtt_PublishJson(MQTTClient *pc, unsigned char *jsonBuf, int len)
{
    int rc = FAILURE;

  MQTTMessage message;

  /* TODO: payload size */
  int offset = 3;
  char *payload = malloc (len + offset);
  if (!payload) {
    return BUFFER_OVERFLOW;
  }
  char *p = payload;
  *p++ = 1;
  *p++ = (len & 0x0000ff00UL) >> 8;
  *p++ = (len & 0x000000ffUL);

  memcpy(p, jsonBuf, len);

    message.qos        = QOS0;
    message.retained   = 0;
    message.payload    = (void*)payload;
    message.payloadlen = len + offset;

  rc = MQTTPublish(pc, "$dp", &message);

  free(payload);
  return rc;
}


int main(int argc, char** argv)
{
  int rc = 0;
  unsigned char buf[1024];
  unsigned char readbuf[1024];

  if (argc < 2)
    usage();

  char* topic = argv[1];

  if (strchr(topic, '#') || strchr(topic, '+'))
    opts.showtopics = 1;
  if (opts.showtopics)
    printf("topic is %s\n", topic);

  getopts(argc, argv);

  Network n;
  MQTTClient c;

  signal(SIGINT, cfinish);
  signal(SIGTERM, cfinish);

  NetworkInit(&n);
  NetworkConnect(&n, opts.host, opts.port);
  MQTTClientInit(&c, &n, 3000, buf, sizeof(buf), readbuf, sizeof(readbuf));
 
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.willFlag = 0;
//  data.MQTTVersion = 3;
  data.MQTTVersion = 4;  /* OneNet must be 4 */
  data.clientID.cstring = opts.clientid;
  data.username.cstring = opts.username;
  data.password.cstring = opts.password;

//  data.keepAliveInterval = 10;
  data.keepAliveInterval = 120;
  data.cleansession = 1;
  printf("Connecting to %s %d\n", opts.host, opts.port);

  rc = MQTTConnect(&c, &data);
  printf("Connected %d\n", rc);
    
  printf("Subscribing to $creq/+\n");
  rc = MQTTSubscribe(&c, (const char *)"$creq/+", opts.qos, messageArrived);
  printf("Subscribed %d\n", rc);

#if 0
  printf("Subscribing to %s\n", topic);
  rc = MQTTSubscribe(&c, topic, opts.qos, messageArrived);
  printf("Subscribed %d\n", rc);
#endif



#if 1
  char *json = "{\"datastreams\":[{\"id\":\"ont_video_1_ubuntu\",\"datapoints\":[{\"value\":{\"dst\":\"video\",\"beginTime\":\"\",\"endTime\":\"\",\"videoDesc\":\"Nothing\"}}]}]}";
  rc = iotMqtt_PublishJson(&c, json, strlen(json));
  if (rc != SUCCESS) {
    printf("Publish Failed: %d\n", rc);
  }
#endif


  while (!toStop)
  {
#if 1
    MQTTMessage message;
    char payload[30];
    static count = 0;

    message.qos = 0;
    message.retained = 0;
    message.payload = payload;
    sprintf(payload, "message number %d", count++);
    message.payloadlen = strlen(payload);

    if ((rc = MQTTPublish(&c, topic, &message)) != 0)
      printf("Return code from MQTT publish is %d\n", rc);
#endif

    MQTTYield(&c, 3000);
  }

  printf("Stopping\n");

  MQTTDisconnect(&c);
  NetworkDisconnect(&n);

  return 0;
}


