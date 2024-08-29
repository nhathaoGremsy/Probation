/*This is program run on PC*/
#include <gst/gst.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

static GstElement *pipeline = NULL;
static GMainLoop *loop = NULL;

/* Signal handler to gracefully stop the pipeline on Ctrl+C */
void handle_signal(int signal) {
    if (signal == SIGINT) {
        g_print("Received SIGINT, sending EOS...\n");
        if (pipeline) {
            /* Send EOS event to the pipeline to finish processing */
            gst_element_send_event(pipeline, gst_event_new_eos());
        }
    }
}

/* Callback function to handle messages on the bus */
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            /* Quit the main loop when EOS is received */
            g_main_loop_quit(loop);
            break;

        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info;

            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");

            g_clear_error(&err);
            g_free(debug_info);

            /* Quit the main loop on error */
            g_main_loop_quit(loop);
            break;
        }

        default:
            break;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    GstBus *bus;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Define the pipeline */
    const char *pipeline_description =
        "udpsrc port=5005 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! "
        "rtph264depay ! tee name=t ! queue ! h264parse ! mp4mux ! filesink location=./received_video.mp4 t. ! queue ! avdec_h264 ! autovideosink" ;

    /* Create the pipeline */
    pipeline = gst_parse_launch(pipeline_description, NULL);

    if (!pipeline) {
        g_printerr("Failed to create pipeline.\n");
        return -1;
    }

    /* Create a GLib Main Loop */
    loop = g_main_loop_new(NULL, FALSE);

    /* Register signal handler for SIGINT (Ctrl+C) */
    signal(SIGINT, handle_signal);

    /* Start playing the pipeline */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Get the bus from the pipeline and add a watch to handle messages asynchronously */
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_call, NULL);
    gst_object_unref(bus);

    /* Run the main loop */
    g_main_loop_run(loop);

    /* Clean up after the main loop exits */
    g_print("Stopping pipeline...\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
