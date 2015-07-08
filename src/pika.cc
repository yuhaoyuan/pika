#include "pika_server.h"
#include "xdebug.h"
#include <glog/logging.h>
#include "pika_conf.h"


#include <iostream>
#include <signal.h>

PikaConf *g_pikaConf;

PikaServer *g_pikaServer;


static void pika_glog_init()
{
    FLAGS_log_dir = g_pikaConf->log_path();
    ::google::InitGoogleLogging("pika");
    FLAGS_minloglevel = g_pikaConf->log_level();
    LOG(WARNING) << "Pika glog init";
    /*
     * dump some usefull message when crash on certain signals
     */
    // google::InstallFailureSignalHandler();
}

static void sig_handler(const int sig)
{
    printf("Caught signal %d", sig);
}

void pika_signal_setup()
{
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    LOG(WARNING) << "pika signal setup ok";
}


static void version()
{
    printf("-----------Pika server 1.0.0----------\n");
}
void pika_init_conf(const char* path)
{
    g_pikaConf = new PikaConf(path);
    if (g_pikaConf == NULL) {
        LOG(FATAL) << "pika load conf error";
    }

    version();
    printf("-----------Pika config list----------\n");
    g_pikaConf->DumpConf();
    printf("-----------Pika config end----------\n");
}


static void usage()
{
    fprintf(stderr,
            "Pika module 1.0.0\n"
            "need One parameters\n"
            "-D the conf path \n"
            "-h show this usage message \n"
            "example: ./bin/pika -D./conf/pika.conf\n"
           );
}

int main(int argc, char **argv)
{
    bool path_opt = false;
    char c;
    char path[PIKA_LINE_SIZE];
    if (argc != 2) {
        usage();
        return -1;
    }
    while (-1 != (c = getopt(argc, argv, "D:h"))) {
        switch (c) {
        case 'D':
            snprintf(path, PIKA_LINE_SIZE, "%s", optarg);
            path_opt = 1;
            break;
        case 'h':
            usage();
            return 0;
        default:
            usage();
            return 0;
        }
    }

    /*
     * check wether set the conf path
     */
    if (path_opt == false) {
        LOG(FATAL) << "Don't get the conf path";
    }

    /*
     * init the conf
     */
    pika_init_conf(path);

    /*
     * init the glog config
     */
    pika_glog_init();

    /*
     * set up the signal
     */
    pika_signal_setup();

    /*
     * Init the server
     */
    g_pikaServer = new PikaServer();
    
    if (g_pikaServer != NULL) {
        LOG(WARNING) << "Pika Server init ok";
    } else {
        LOG(FATAL) << "Pika Server init error";
    }


    LOG(WARNING) << "Pika Server going to start";
    g_pikaServer->RunProcess();

    return 0;
}
