Config file format
==================

This file describes the xml format of the config file.

Cameras Section
---------------
The cameras section is marked by a surrounding `<cameras> ... </cameras>` tag.

Inside that tag can be  a arbitrary amount of `<camera>` tags. Each `<camera>` tags has 3 mandatory child tags.

 * `<requestedWidth>` - the requested width of the camera
 * `<requestedHeight>` - the requested height of the camera
 * `<id>` - the device id the operating system gives the camera (usually an index started at 0)

The width and height are only requests to the camera. Not all cameras support all resolutions. 
Sometimes camera chooses a lower resolution it supports. Sometimes the camera initialization fails
if the camera doesn't support the requested resolution. Make sure to check the console.
640x480 seems to be a resolution most cameras support. 
(For example my Logitech C310 supports the following resolutions: (4:3) [640x480, 800x600, 1280x960] and (16:9) [1024x576, 1280x720])

The `<camera>` tags also has one optional child tag:

 * `<useForSnapshotBackround>` - if this tag contains a `1` then this camera will be used to take a picture for the snapshot background

If `<useForSnapshotBackround>` is set to `1` in multiple cameras, the last one will be taken. 
Per default, if this tag is not defined in any camera, the first camera will be taken.

#### Example code:
``` xml
<cameras>
    <camera>
        <requestedWidth>800</requestedWidth>
        <requestedHeight>600</requestedHeight>
        <id>0</id>
    </camera>
    <camera>
        <requestedWidth>640</requestedWidth>
        <requestedHeight>480</requestedHeight>
        <id>1</id>
        <useForSnapshotBackround>1</useForSnapshotBackround>
    </camera>
</cameras>
```