#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#define MP4_FILE_PATH "/media/file_example_MP4_1920_18MG.mp4"  // Replace with the path to your MP4 file

int main(int argc, char *argv[]) {
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
    gchar *pipeline;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create a main loop
    loop = g_main_loop_new(NULL, FALSE);

    // Create an RTSP server instance
    server = gst_rtsp_server_new();


    gst_rtsp_server_set_address(server, "192.168.12.168");
    gst_rtsp_server_set_service(server, "8554");

    // Get the default mount points from the server
    mounts = gst_rtsp_server_get_mount_points(server);

    // Create a media factory for the server
    factory = gst_rtsp_media_factory_new();

    // Define the pipeline to stream the MP4 file
    pipeline = g_strdup_printf(
        "( filesrc location=%s ! qtdemux ! h264parse  ! rtph264pay name=pay0 pt=96 )",
        MP4_FILE_PATH
    );
    gst_rtsp_media_factory_set_launch(factory, pipeline);

    // Attach the factory to the "/stream" URL path
    gst_rtsp_mount_points_add_factory(mounts, "/stream", factory);

    // Attach the server to the default main context
    gst_rtsp_server_attach(server, NULL);

    // Print the RTSP stream URL
    g_print("RTSP server is running at rtsp://192.168.12.168:8554/stream\n");

    // Start the main loop
    g_main_loop_run(loop);

    // Free the pipeline string
    g_free(pipeline);

    return 0;
}
