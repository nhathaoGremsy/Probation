/*This is program run on s5l*/
#include <gst/gst.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
static GstElement *pipeline = NULL;

void handle_signal(int signal) {
    if (signal == SIGINT) {
        g_print("Received SIGINT, stopping pipeline...\n");
        if (pipeline) {
            gst_element_set_state(pipeline, GST_STATE_NULL);
        }
        exit(0);
    }
}
int main(int argc, char *argv[])
{
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Define the pipeline using a string with gst_parse_launch */
    const char *pipeline_description =
        "gst-launch-1.0 olcamerasrc ! video/x-h264,width=1920,height=1080,stream-format=byte-stream ! queue ! rtph264pay ! udpsink host=192.168.12.43 port=5005";

    /* Create the pipeline using gst_parse_launch */
    pipeline = gst_parse_launch(pipeline_description, NULL);

    if (!pipeline)
    {
        g_printerr("Failed to create pipeline.\n");
        return -1;
    }

    /* Register signal handler for SIGINT */
    signal(SIGINT, handle_signal);

    /* Start playing the pipeline */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Free resources */
    if (msg != NULL)
    {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
