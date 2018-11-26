/* mqttexample.c
 *
 * Copyright (C) 2006-2018 wolfSSL Inc.
 *
 * This file is part of wolfMQTT.
 *
 * wolfMQTT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfMQTT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/* Include the autoconf generated config.h */
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "wolfmqtt/mqtt_client.h"
#include "examples/mqttexample.h"
#include "examples/mqttnet.h"


/* locals */
static int mPacketIdLast;

/* argument parsing */
static int myoptind = 0;
static char* myoptarg = NULL;

#ifdef ENABLE_MQTT_TLS
	const unsigned char caCertBuf[] =
	/* DigiCert Baltimore Root */
	"-----BEGIN CERTIFICATE-----\r\n"
	"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
	"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n"
	"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
	"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n"
	"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
	"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n"
	"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
	"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n"
	"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
	"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n"
	"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
	"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n"
	"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
	"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n"
	"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
	"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n"
	"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
	"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n"
	"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n"
	"-----END CERTIFICATE-----\r\n"
	;
	// MyTestServer
	/*const unsigned char caCertBuf[] =
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
	;*/
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

#define MY_EX_USAGE 2 /* Exit reason code */
static int mygetopt(int argc, char** argv, const char* optstring)
{
    static char* next = NULL;

    char  c;
    char* cp;

    if (myoptind == 0)
        next = NULL;   /* we're starting new/over */

    if (next == NULL || *next == '\0') {
        if (myoptind == 0)
            myoptind++;

        if (myoptind >= argc || argv[myoptind][0] != '-' ||
                                argv[myoptind][1] == '\0') {
            myoptarg = NULL;
            if (myoptind < argc)
                myoptarg = argv[myoptind];

            return -1;
        }

        if (XSTRNCMP(argv[myoptind], "--", 2) == 0) {
            myoptind++;
            myoptarg = NULL;

            if (myoptind < argc)
                myoptarg = argv[myoptind];

            return -1;
        }

        next = argv[myoptind];
        next++;                  /* skip - */
        myoptind++;
    }

    c  = *next++;
    /* The C++ strchr can return a different value */
    cp = (char*)XSTRCHR(optstring, c);

    if (cp == NULL || c == ':')
        return '?';

    cp++;

    if (*cp == ':') {
        if (*next != '\0') {
            myoptarg = next;
            next     = NULL;
        }
        else if (myoptind < argc) {
            myoptarg = argv[myoptind];
            myoptind++;
        }
        else
            return '?';
    }

    return c;
}

void mqtt_show_usage(MQTTCtx* mqttCtx)
{
    PRINTF("%s:", mqttCtx->app_name);
    PRINTF("-?          Help, print this usage");
    PRINTF("-h <host>   Host to connect to, default %s",
        mqttCtx->host);
#ifdef ENABLE_MQTT_TLS
    PRINTF("-p <num>    Port to connect on, default: Normal %d, TLS %d",
        MQTT_DEFAULT_PORT, MQTT_SECURE_PORT);
    PRINTF("-t          Enable TLS");
    PRINTF("-c <file>   Use provided certificate file");
#else
    PRINTF("-p <num>    Port to connect on, default: %d",
        MQTT_DEFAULT_PORT);
#endif
    PRINTF("-q <num>    Qos Level 0-2, default %d",
        mqttCtx->qos);
    PRINTF("-s          Disable clean session connect flag");
    PRINTF("-k <num>    Keep alive seconds, default %d",
        mqttCtx->keep_alive_sec);
    PRINTF("-i <id>     Client Id, default %s",
        mqttCtx->client_id);
    PRINTF("-l          Enable LWT (Last Will and Testament)");
    PRINTF("-u <str>    Username");
    PRINTF("-w <str>    Password");
    PRINTF("-n <str>    Topic name, default %s", mqttCtx->topic_name);
    PRINTF("-r          Set Retain flag on publish message");
    PRINTF("-C <num>    Command Timeout, default %dms", mqttCtx->cmd_timeout_ms);
    PRINTF("-T          Test mode");
    if (mqttCtx->pub_file) {
	    PRINTF("-f <file>   Use file for publish, default %s",
	        mqttCtx->pub_file);
	}
}

void mqtt_init_ctx(MQTTCtx* mqttCtx)
{
    XMEMSET(mqttCtx, 0, sizeof(MQTTCtx));
    mqttCtx->host = DEFAULT_MQTT_HOST;
    mqttCtx->qos = DEFAULT_MQTT_QOS;
    mqttCtx->clean_session = 1;
    mqttCtx->keep_alive_sec = DEFAULT_KEEP_ALIVE_SEC;
    mqttCtx->client_id = DEFAULT_CLIENT_ID;
    mqttCtx->topic_name = DEFAULT_TOPIC_NAME;
    mqttCtx->cmd_timeout_ms = DEFAULT_CMD_TIMEOUT_MS;
}

int mqtt_parse_args(MQTTCtx* mqttCtx, int argc, char** argv)
{
	int rc;

	#ifdef ENABLE_MQTT_TLS
		#define MQTT_TLS_ARGS "c:"
	#else
		#define MQTT_TLS_ARGS ""
	#endif

    while ((rc = mygetopt(argc, argv, "?h:p:q:sk:i:lu:w:n:C:Tf:rt" MQTT_TLS_ARGS)) != -1) {
        switch ((char)rc) {
        case '?' :
            mqtt_show_usage(mqttCtx);
            return MY_EX_USAGE;

        case 'h' :
            mqttCtx->host = myoptarg;
            break;

        case 'p' :
            mqttCtx->port = (word16)XATOI(myoptarg);
            if (mqttCtx->port == 0) {
                return err_sys("Invalid Port Number!");
            }
            break;

        case 'q' :
            mqttCtx->qos = (MqttQoS)((byte)XATOI(myoptarg));
            if (mqttCtx->qos > MQTT_QOS_2) {
                return err_sys("Invalid QoS value!");
            }
            break;

        case 's':
            mqttCtx->clean_session = 0;
            break;

        case 'k':
            mqttCtx->keep_alive_sec = XATOI(myoptarg);
            break;

        case 'i':
            mqttCtx->client_id = myoptarg;
            break;

        case 'l':
            mqttCtx->enable_lwt = 1;
            break;

        case 'u':
            mqttCtx->username = myoptarg;
            break;

        case 'w':
            mqttCtx->password = myoptarg;
            break;

        case 'n':
            mqttCtx->topic_name = myoptarg;
            break;

        case 'C':
            mqttCtx->cmd_timeout_ms = XATOI(myoptarg);
            break;

        case 'T':
            mqttCtx->test_mode = 1;
            break;

        case 'f':
	        mqttCtx->pub_file = myoptarg;
	        break;

		case 'r':
	        mqttCtx->retain = 1;
	        break;

        case 't':
            mqttCtx->use_tls = 1;
            break;

    #ifdef ENABLE_MQTT_TLS
        case 'c':
            mTlsCaFile = myoptarg;
            break;
	#endif

        default:
            mqtt_show_usage(mqttCtx);
            return MY_EX_USAGE;
        }
    }

    myoptind = 0; /* reset for test cases */

    /* if TLS not enable, check args */
#ifndef ENABLE_MQTT_TLS
    if (mqttCtx->use_tls) {
        PRINTF("Use TLS option not allowed (TLS not compiled in)");
        mqttCtx->use_tls = 0;
        if (mqttCtx->test_mode) {
            return MY_EX_USAGE;
        }
    }
#endif

    return 0;
}

#if defined(__GNUC__) && !defined(NO_EXIT)
     __attribute__ ((noreturn))
#endif
int err_sys(const char* msg)
{
    if (msg) {
        PRINTF("wolfMQTT error: %s", msg);
    }
	exit(EXIT_FAILURE);
}


word16 mqtt_get_packetid(void)
{
    mPacketIdLast = (mPacketIdLast >= MAX_PACKET_ID) ?
        1 : mPacketIdLast + 1;
    return (word16)mPacketIdLast;
}

#ifdef WOLFMQTT_NONBLOCK
#if defined(MICROCHIP_MPLAB_HARMONY)
    #include <system/tmr/sys_tmr.h>
#else
    #include <time.h>
#endif

static word32 mqtt_get_timer_seconds(void)
{
    word32 timer_sec = 0;

#if defined(MICROCHIP_MPLAB_HARMONY)
    timer_sec = (word32)(SYS_TMR_TickCountGet() /
                         SYS_TMR_TickCounterFrequencyGet());
#else
    /* Posix style time */
    timer_sec = (word32)time(0);
#endif

    return timer_sec;
}

int mqtt_check_timeout(int rc, word32* start_sec, word32 timeout_sec)
{
    word32 elapsed_sec;

    /* if start seconds not set or is not continue */
    if (*start_sec == 0 || rc != MQTT_CODE_CONTINUE) {
        *start_sec = mqtt_get_timer_seconds();
        return rc;
    }

    elapsed_sec = mqtt_get_timer_seconds();
    if (*start_sec < elapsed_sec) {
        elapsed_sec -= *start_sec;
        if (elapsed_sec >= timeout_sec) {
            *start_sec = mqtt_get_timer_seconds();
            return MQTT_CODE_ERROR_TIMEOUT;
        }
    }

    return rc;
}
#endif /* WOLFMQTT_NONBLOCK */


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
int mqtt_tls_cb(MqttClient* client)
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
	    if (mTlsCaFile) {
	    #if 0
	    	/* As example, load file into buffer for testing */
	    	long  caCertSize = 0;
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
	    #endif
	    }

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
