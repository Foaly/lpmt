LPMT - Little Projection-Mapping Tool
=====================================

LPMT is a little projection-mapping tool, which was initially developed at the Hermanitos Verdes Architetti office in Modena, Italy. 
It is developed in C++ using OpenFrameworks. 
It’s based on simple quad warping paradigm, and, though rather simple, can
be used to achieve complex and professional projection-mapping sets.

Since the original repo seems to be discontinued I forked the repo and started to add my own modifications.


Highlights of this Fork:
========================

 * Completely rewrote the GUI code
   * Instead of 3 pages for each of the 36 quads (108 pages!) we only have 3 pages now, whos content gets updated when the selected quad changes
   * A nice header bar with a tab for each page was added (currently selected page is highlighted)
   * Lots of bugfixes in the GUI controls (ComboBox works again, improved selection in ColorPicker and ComboBox, ...)
 * Reviewed all the blend modes to look like GIMP/Photoshop and also added some new ones (see `docs/blendmodes.md`)
 * In non-gui mode doubleclick anywhere on a quad to activate/select it
 * Fixed a nasty bug, where the mask markers where not displayed correctly after being loaded from a project file to a screen with a different size
 * Let the user choose which camera to use for the snapshot background via the config.xml file (see `docs/config file format.md`)
 * Compiles  with the latest version of openframeworks (0.8.4) and all the addons it depends on

 * A ton of bugfixes and code refactoring for example:
   * refactored load and save project (also added GUI buttons)
   * reworked camera initialization code to be less error prone
   * fixed a bug where LPMT would crash if "w" was pressed when no camera was connected
   * lots of small refactors
   * for a more complete list check the commit log and the issues

 * LOTS of code clean up 


Download:
=========

At the moment LPMT is only available as source code. It is tested and known to work on linux und windows systems. 
Thanks to the multi-platform nature of OpenFrameworks it should work out-of-the-box on OSX too.

Up-to-date code of this repository:
https://github.com/Foaly/lpmt

For reference the original repository is here:
https://github.com/hvfrancesco/lpmt


Dependencies:
=============
If you want to compile the program from source, you need to download the following dependencies. Simply download the repos to your `your_openframworks_root_directory/addons` directory.
 * ofxMostPixelsEver (https://github.com/obviousjim/ofxMostPixelsEver)
 * my fork (important!) of ofxSimpleGuiToo (https://github.com/Foaly/ofxSimpleGuiToo)
   * make sure to run the `clone_dependencies.sh` script to install all of it's dependencies (ofxTextInputField, ofxMSAInteractiveObject, ofxMidi)
 * ofxTimeline (https://github.com/YCAMInterlab/ofxTimeline/)
   * make sure to run the `clone_addons.sh` file to install all of it's dependencies (ofxTween, ofxRange, ofxMSATimer, ofxTimecode, ofxTextInputField)
 * ofxMidi (https://github.com/danomatika/ofxMidi) 


Main Features:
==============

* up to 36 independent projection surfaces (remember you can use a solid black quad even as a mask)
* possible content: solid color, images, video, live-cam, slideshows, smoothly changing solid colors, and more …
* customizable green-screen (chromakey) for video and live-cam sources
* horizontal and vertical flipping for image, video and live-cam content
* independent opacity settings for each content element
* customizable colorization for each content element
* several blending-modes for projection surfaces
* on-screen editable masks
* bezier-warping for projection on curved surfaces
* grid-warping, with user defined grid density
* spherize deformation preset
* realtime live-masking with Kinect
* customizable video speed and volume
* save/load project set to/from xml file
* synchronized start for video elements
* vertex snap function for adjacent quads
* camera snapshot background for rough positioning of projection surfaces
* customizable speed for slideshows and color transitions
* synced multi-projectors shows with adjustable edge-blending
* control and setup through a rich set of OSC messages
* a powerful timeline to trigger events and control LPMT projections


Documentation:
==============

* Key Commands: take a look at the `bin/data/help_keys.txt` file
* Files in the `doc/` directory


Contact:
========

Contact to me:
 * Just write and issue here on this repo
 * https://github.com/Foaly/lpmt

Contact to the owner of the original repo:
 * francesco[at]hv-a.com
 * www.hv-a.com
 * https://github.com/hvfrancesco/lpmt
