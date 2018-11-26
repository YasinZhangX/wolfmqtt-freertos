/* Wolf Includes */
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfmqtt/mqtt_client.h"
#include "examples/mqttnet.h" /* example FreeRTOS TCP network callbacks */
#include "wolfssl/ssl.h"

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_Sockets.h"


/* Configuration */
#define MQTT_BUF_SIZE	1024
#define DEFAULT_MQTT_HOST       "www.yasinzhang.xyz" //"iot.eclipse.org"  /* broker.hivemq.com */
#define DEFAULT_CMD_TIMEOUT_MS  30000
#define DEFAULT_CON_TIMEOUT_MS  50000
#define DEFAULT_MQTT_QOS        MQTT_QOS_0
#define DEFAULT_KEEP_ALIVE_SEC  60
#define DEFAULT_USERNAME		"yasin"
#define DEFAULT_USERPW			"testserver"
#define DEFAULT_CLIENT_ID       "WolfMQTTClient"
#define WOLFMQTT_TOPIC_NAME     "wolfMQTT/example/"
#define DEFAULT_TOPIC_NAME      WOLFMQTT_TOPIC_NAME"testTopic"
#define TLS_CA_CERT             "DSTRootCAX3.pem"

#ifdef ENABLE_MQTT_TLS
//const unsigned char caCertBuf[] =
///* DigiCert Baltimore Root */
//"-----BEGIN CERTIFICATE-----\r\n"
//"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
//"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n"
//"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
//"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n"
//"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
//"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n"
//"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
//"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n"
//"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
//"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n"
//"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
//"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n"
//"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
//"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n"
//"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
//"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n"
//"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
//"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n"
//"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n"
//"-----END CERTIFICATE-----\r\n"
//;
// MyTestServer
const unsigned char caCertBuf[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDpzCCAo+gAwIBAgIJAMGVRewzNkTFMA0GCSqGSIb3DQEBDQUAMGoxFzAVBgNV\r\n"
"BAMMDkFuIE1RVFQgYnJva2VyMRYwFAYDVQQKDA1Pd25UcmFja3Mub3JnMRQwEgYD\r\n"
"VQQLDAtnZW5lcmF0ZS1DQTEhMB8GCSqGSIb3DQEJARYSbm9ib2R5QGV4YW1wbGUu\r\n"
"bmV0MB4XDTE4MTEyMzA4MjgwMVoXDTMyMTExOTA4MjgwMVowajEXMBUGA1UEAwwO\r\n"
"QW4gTVFUVCBicm9rZXIxFjAUBgNVBAoMDU93blRyYWNrcy5vcmcxFDASBgNVBAsM\r\n"
"C2dlbmVyYXRlLUNBMSEwHwYJKoZIhvcNAQkBFhJub2JvZHlAZXhhbXBsZS5uZXQw\r\n"
"ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDxHHEWEIuz949x6kCno2Au\r\n"
"gETazyAb3AlB7leuukrmrAUuj0AQCyutJ5QquAohIZ3EZaiSV5HxFggIZl8z2uZ7\r\n"
"WzeeuRpxG5Imi8EAHtu850ybOyWz0yL9OXHs6lwoBtnEjLvX7C6040u2ADyNuwRn\r\n"
"pAICRneiEHZunrGTE2q4oLwqeP1y4on9SGmoR4NlclF0WhDse5IOADcpQT88BTZc\r\n"
"d2ZBSIZYK5+LjGJgOjL5mB74j+qtktQD4SxAxNndP7iKdTkEXYuVWiK2MT8pgvX8\r\n"
"U8AmtN9ocWdjvL0iTBIwBW3VBv+QQDkvrKZb0CwZ/EIf1imlN2lHYHyULnBq6NX/\r\n"
"AgMBAAGjUDBOMB0GA1UdDgQWBBT9OzQrhPqRcb/egvw4A+0/yLG9vjAfBgNVHSME\r\n"
"GDAWgBT9OzQrhPqRcb/egvw4A+0/yLG9vjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3\r\n"
"DQEBDQUAA4IBAQAEt2mdFKcC4vOvpcfOhTM6LK4WgmBQmMqY8LIDVzjuqvQUbulb\r\n"
"NaT7/DckSWGuUas8DDPviTu8IVmPN3Cjkal52s6n8cORTHJeRKrW+PMGtNxUjyqf\r\n"
"26QI67bL3dmvO1GQWsvNLQArjdivPFBThXa4DRQviIT2s9KK37NXOz0b/dCG1yOj\r\n"
"2x3QU8Jo1daRztJPQKBG+1ba3nVG2nwcb+MRrWmCnPEj1iw92EkP68Qd/hckYIhO\r\n"
"3YQ1V2GGmx+M2JaPg5xLk0KOsxcPtw1MxPObLZ14bJ8CQ7m+NOvMPBAeK8cdcHyT\r\n"
"+FTCboaVd+7zgfo5fq9b1jNpOVsAmjhVaKWt\r\n"
"-----END CERTIFICATE-----\r\n"
;
// DongMing-Server
//const unsigned char caCertBuf[] =
//"-----BEGIN CERTIFICATE-----\r\n"
//"MIID0TCCArmgAwIBAgIJAPVG6FHon2kdMA0GCSqGSIb3DQEBCwUAMH4xCzAJBgNV\r\n"
//"BAYTAkNOMREwDwYDVQQIDAhTaWNodWFuZzELMAkGA1UEBwwCQ0QxDjAMBgNVBAoM\r\n"
//"BXVlc3RjMQwwCgYDVQQLDAMyMTgxEDAOBgNVBAMMBzAuMC4wLjAxHzAdBgkqhkiG\r\n"
//"9w0BCQEWEDU2ODgyMjUwN0BxcS5jb20wIBcNMTgxMTIyMDkwMDMzWhgPMjExODEw\r\n"
//"MjkwOTAwMzNaMH4xCzAJBgNVBAYTAkNOMREwDwYDVQQIDAhTaWNodWFuZzELMAkG\r\n"
//"A1UEBwwCQ0QxDjAMBgNVBAoMBXVlc3RjMQwwCgYDVQQLDAMyMTgxEDAOBgNVBAMM\r\n"
//"BzAuMC4wLjAxHzAdBgkqhkiG9w0BCQEWEDU2ODgyMjUwN0BxcS5jb20wggEiMA0G\r\n"
//"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDY0I9P51GZXSpAOJZu/4/E/IrBuwOg\r\n"
//"fU2gwszNnY3Wf9uD6C1rYnNYji1jMImbv4o729VfEi/rQgc4EVwWkPvNlOYFKq4q\r\n"
//"XVoL74sPD7UaWfLiFmsCEvc/pn60cIQtShQYKp12guEO1TKRXdjqVcvBbe+pzEWl\r\n"
//"UY1zLrsKhCLbE010wvTLFm7qHdRYH9VbNktlqqxEtVvIkQd+dfUVDb7mBxLsJjZb\r\n"
//"a6VWbPvaaKIGS5QZx7X6zswAb3Y2fnhCIYRRY/u6tPYcRXjVUz6RewarQVvv1cOA\r\n"
//"YEuFfBNR18n7jIlAXFzr5nXBorGYD3EWYVRjXlk81mmGQBMO8+nlhFwFAgMBAAGj\r\n"
//"UDBOMB0GA1UdDgQWBBTlWNm7WDHq+t38bU8hVrs3I1Lq4zAfBgNVHSMEGDAWgBTl\r\n"
//"WNm7WDHq+t38bU8hVrs3I1Lq4zAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUA\r\n"
//"A4IBAQBoKMJfWK769LZohzf87IDHgYjyASvCU0qr1vO6k+DSRpFWfc55pvVQL4N5\r\n"
//"2c34iardcICjr8L+46guSvLFSRaDwVs2r5LcOo2L4/XTx7dD23J5Ai/2uG8v5YC2\r\n"
//"DgxBEoqXzWUVj3tpdBX5ObxQfVTcWnGajkl/3OTDGMAho3p7L4PSLxAp5EHSO9mD\r\n"
//"7KiPdwEHqglw3XPa98NMjJPEqDp5NQ8BipUK2p/K4ZdRX1qwgKITz0SC01Lcw+9U\r\n"
//"mATJSAay0gxlPtCVSxv5yHe67qvwy+qVNaXq+o4DT1WaPNULQhQWiCKob3LGTwIX\r\n"
//"ros2+RnMkZnhE5V2zdGUUnl8vlxl\r\n"
//"-----END CERTIFICATE-----\r\n"
//;
static const char* mTlsCaFile = caCertBuf;
#endif

static MqttClient gMQTTC;
static MqttNet gMQTTN;
static byte	gMqttTxBuf[MQTT_BUF_SIZE];
static byte	gMqttRxBuf[MQTT_BUF_SIZE];
static int mPacketIdLast;
//static const char* mTlsCaFile = TLS_CA_CERT;


#define PRINT_BUFFER_SIZE 1024
static int mqtt_message_cb(MqttClient *client, MqttMessage *msg,
    byte msg_new, byte msg_done)
{
    byte buf[PRINT_BUFFER_SIZE+1];
    word32 len;

	(void)client;

    if (msg_new) {
        /* Determine min size to dump */
        len = msg->topic_name_len;
        if (len > PRINT_BUFFER_SIZE) {
            len = PRINT_BUFFER_SIZE;
        }
        XMEMCPY(buf, msg->topic_name, len);
        buf[len] = '\0'; /* Make sure its null terminated */

        /* Print incoming message */
        PRINTF("MQTT Message: Topic %s, Qos %d, Len %u",
            buf, msg->qos, msg->total_len);
    }

    /* Print message payload */
    len = msg->buffer_len;
    if (len > PRINT_BUFFER_SIZE) {
        len = PRINT_BUFFER_SIZE;
    }
    XMEMCPY(buf, msg->buffer, len);
    buf[len] = '\0'; /* Make sure its null terminated */
    PRINTF("Payload (%d - %d): %s",
        msg->buffer_pos, msg->buffer_pos + len, buf);

    if (msg_done) {
        PRINTF("MQTT Message: Done");
    }

    /* Return negative to terminate publish processing */
    return MQTT_CODE_SUCCESS;
}

#define MAX_PACKET_ID ((1 << 16) - 1)
static word16 mqttclient_get_packetid(void)
{
    mPacketIdLast = (mPacketIdLast >= MAX_PACKET_ID) ? 1 : mPacketIdLast + 1;
    return (word16)mPacketIdLast;
}

#ifdef ENABLE_MQTT_TLS
static int mqtt_tls_verify_cb(int preverify, WOLFSSL_X509_STORE_CTX* store)
{
    char buffer[WOLFSSL_MAX_ERROR_SZ];

    PRINTF("MQTT TLS Verify Callback: PreVerify %d, Error %d (%s)", preverify,
        store->error, store->error != 0 ?
            wolfSSL_ERR_error_string(store->error, buffer) : "none");
    PRINTF("  Subject's domain name is %s", store->domain);

    if (store->error != 0) {
        /* Allowing to continue */
        /* Should check certificate and return 0 if not okay */
        PRINTF("  Allowing cert anyways");
    }

    return 1;
}

/* Use this callback to setup TLS certificates and verify callbacks */
static int mqtt_tls_cb(MqttClient* client)
{
    int rc = WOLFSSL_FAILURE;

    client->tls.ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (client->tls.ctx) {
        wolfSSL_CTX_set_verify(client->tls.ctx, WOLFSSL_VERIFY_PEER,
                               mqtt_tls_verify_cb);

        /* default to success */
        rc = WOLFSSL_SUCCESS;

    #if !defined(NO_CERT)
    #if !defined(NO_FILESYSTEM)
		if (mTlsCaFile) {
			/* Load CA certificate file */
			//rc = wolfSSL_CTX_load_verify_locations(client->tls.ctx, mTlsCaFile, NULL);
			rc = wolfSSL_CTX_load_verify_buffer(client->tls.ctx, caCertBuf,
				sizeof(caCertBuf), WOLFSSL_FILETYPE_PEM);
		}

        /* If using a client certificate it can be loaded using: */
        /* rc = wolfSSL_CTX_use_certificate_file(client->tls.ctx,
         *                              clientCertFile, WOLFSSL_FILETYPE_PEM);*/

    #else
        /* Load CA certificate buffer */
        rc = wolfSSL_CTX_load_verify_buffer(client->tls.ctx, caCertBuf,
                                          caCertBufLen, WOLFSSL_FILETYPE_PEM);

        #if 0
        if (mTlsCaFile) {
            long  caCertSize = 0;
            /* As example, load file into buffer for testing */
            byte  caCertBuf[10000];
            FILE* file = fopen(mTlsCaFile, "rb");
            if (!file) {
                err_sys("can't open file for CA buffer load");
            }
            fseek(file, 0, SEEK_END);
            caCertSize = ftell(file);
            rewind(file);
            fread(caCertBuf, sizeof(caCertBuf), 1, file);
            fclose(file);

            /* Load CA certificate buffer */
            rc = wolfSSL_CTX_load_verify_buffer(client->tls.ctx, caCertBuf,
                                              caCertSize, WOLFSSL_FILETYPE_PEM);
        }
        #endif


        /* If using a client certificate it can be loaded using: */
        /* rc = wolfSSL_CTX_use_certificate_buffer(client->tls.ctx,
         *               clientCertBuf, clientCertSize, WOLFSSL_FILETYPE_PEM);*/
    #endif /* !NO_FILESYSTEM */
    #endif /* !NO_CERT */
    }

    PRINTF("MQTT TLS Setup (%d)", rc);

    return rc;
}
#else
int mqtt_tls_cb(MqttClient* client)
{
    (void)client;
    return 0;
}
#endif /* ENABLE_MQTT_TLS */


void* vSecureMQTTClientTask( void *pvParameters )
{
    int rc;
    int state = -1;
    word32 cntr = 0;
    MqttConnect connect;
    MqttMessage lwt_msg;
    MqttPublish publish;
    char PubMsg[16];

	(void)pvParameters;

    PRINTF("Starting MQTT");

    for(;;) {

        /* setup network callbacks */
        rc = MqttClientNet_Init(&gMQTTN);
        PRINTF("MQTT Net Init: %s (%d)",
            MqttClient_ReturnCodeToString(rc), rc);
        if (rc != MQTT_CODE_SUCCESS) {
			break;
        }

        rc = MqttClient_Init(&gMQTTC, &gMQTTN,
                mqtt_message_cb,
                gMqttTxBuf, MQTT_BUF_SIZE,
                gMqttRxBuf, MQTT_BUF_SIZE,
                DEFAULT_CMD_TIMEOUT_MS);
        PRINTF("MQTT Init: %s (%d)",
            MqttClient_ReturnCodeToString(rc), rc);
		if (rc != MQTT_CODE_SUCCESS)
			break;

        state = 0;

		/* trim trailing zeros for sub-counter */
        cntr/=100000; cntr*=100000;

        while ((rc == MQTT_CODE_SUCCESS) || (rc == MQTT_CODE_CONTINUE)) {
            switch (state) {
            case 0:
            {
                rc = MqttClient_NetConnect(&gMQTTC, DEFAULT_MQTT_HOST, 0,
                        DEFAULT_CON_TIMEOUT_MS, 1, mqtt_tls_cb);

                if (rc != MQTT_CODE_SUCCESS) {
                    vTaskDelay(250);
                    PRINTF("NetConnect continue(%d)...", rc);
                    break;
                }
                XMEMSET(&connect, 0, sizeof(connect));
                connect.keep_alive_sec = DEFAULT_KEEP_ALIVE_SEC;

                connect.clean_session = 1;
                connect.client_id = DEFAULT_CLIENT_ID;
                connect.username = DEFAULT_USERNAME;
                connect.password = DEFAULT_USERPW;

                XMEMSET(&lwt_msg, 0, sizeof(lwt_msg));
                connect.enable_lwt = 0;
                connect.lwt_msg = &lwt_msg;
            }

            FALL_THROUGH;
            case 1:
            {
                state = 1;

                rc = MqttClient_Connect(&gMQTTC, &connect);
                PRINTF("MQTT Connect: %s (%d)",
                    MqttClient_ReturnCodeToString(rc), rc);

                if (rc == MQTT_CODE_CONTINUE) {
                    vTaskDelay(250);
                    PRINTF("Connect continue...");
                    break;
                }
                else if (rc == MQTT_CODE_SUCCESS) {
                    state = 2;
                }
                break;
            }
            case 2:
            {
                MqttSubscribe subscribe;
                MqttTopic topics[1];

                /* Build list of topics */
                XMEMSET(topics, 0, sizeof(topics));
                topics[0].topic_filter = DEFAULT_TOPIC_NAME;
                topics[0].qos = MQTT_QOS_0;

                /* Subscribe Topic */
                XMEMSET(&subscribe, 0, sizeof(MqttSubscribe));
                subscribe.packet_id = mqttclient_get_packetid();
                subscribe.topic_count = sizeof(topics)/sizeof(MqttTopic);
                subscribe.topics = topics;
                rc = MqttClient_Subscribe(&gMQTTC, &subscribe);
                PRINTF("MQTT Subscribe: %s (%d)",
                    MqttClient_ReturnCodeToString(rc), rc);

                state = 3;
                break;
            }
            case 3:
            {
                rc = MqttClient_WaitMessage(&gMQTTC, 1000);
                if (rc == MQTT_CODE_ERROR_TIMEOUT) {
                    /* A timeout is not an error, it just means there is no data */
                    rc = MQTT_CODE_SUCCESS;
                }

                if (rc == MQTT_CODE_SUCCESS) {
                    cntr++; /* increment counter */

                    XSNPRINTF(PubMsg, sizeof(PubMsg), "Counter:%d", (int)cntr);

                    /* Publish Topic */
                    XMEMSET(&publish, 0, sizeof(publish));
                    publish.retain = 0;
                    publish.qos = DEFAULT_MQTT_QOS;
                    publish.duplicate = 0;
                    publish.topic_name = DEFAULT_TOPIC_NAME;
                    publish.packet_id = mqttclient_get_packetid();
                    publish.buffer = (byte*)PubMsg;
                    publish.total_len = (word16)XSTRLEN(PubMsg);
                    rc = MqttClient_Publish(&gMQTTC, &publish);
                    PRINTF("MQTT Publish: Topic %s, %s (%d)",
                        publish.topic_name,
                        MqttClient_ReturnCodeToString(rc), rc);
                }
                break;
            }
            default:
                break;
            } /* switch */

            if ((rc != MQTT_CODE_SUCCESS) && (rc != MQTT_CODE_CONTINUE)) {
                PRINTF("Disconnect: State %d, %s (%d)",
                    state, MqttClient_ReturnCodeToString(rc), rc);

                MqttClient_NetDisconnect(&gMQTTC);
                break;
            }
        } /* while loop */

        PRINTF("While break: %s (%d)",
            MqttClient_ReturnCodeToString(rc), rc);

        cntr += 100000;
        vTaskDelay(5000);
    } /* for loop */

    return (void*)rc;
}
