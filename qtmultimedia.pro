load(configure)
qtCompileTest(openal)
win32 {
    qtCompileTest(directshow)
    qtCompileTest(wmsdk)
    qtCompileTest(wmp)
    qtCompileTest(wmf)
    qtCompileTest(evr)
} else:mac {
    qtCompileTest(avfoundation)
} else {
    qtCompileTest(alsa)
    qtCompileTest(pulseaudio)
    !done_config_gstreamer {
        gstver=1.0
        cache(GST_VERSION, set, gstver);
        qtCompileTest(gstreamer) {
            qtCompileTest(gstreamer_photography)
            qtCompileTest(gstreamer_encodingprofiles)
            qtCompileTest(gstreamer_appsrc)
        } else {
            gstver=0.10
            cache(GST_VERSION, set, gstver);
            # Force a re-run of the test
            CONFIG -= done_config_gstreamer
            qtCompileTest(gstreamer) {
                qtCompileTest(gstreamer_photography)
                qtCompileTest(gstreamer_encodingprofiles)
                qtCompileTest(gstreamer_appsrc)
            }
        }
    }
    qtCompileTest(resourcepolicy)
    equals(GST_VERSION, 0.10) {
        qtCompileTest(xvideo)
    }
}

load(qt_parts)

