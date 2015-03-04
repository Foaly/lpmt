Blendmodes
==========

The following OpenGL Marcos are used to acchive the blend mode effects.
Taken from these sites:
 * http://www.pegtop.net/delphi/articles/blendmodes/index.htm
 * https://github.com/LaurentGomila/SFML/blob/master/src/SFML/Graphics/BlendMode.cpp
 * http://wiki.delphigl.com/index.php/glBlendFunc
 * http://gamedev.stackexchange.com/questions/17043/blend-modes-in-cocos2d-with-glblendfunc
 * https://github.com/processing/processing/blob/master/core/src/processing/opengl/PGraphicsOpenGL.java#L6029


None
----
glBlendFunc(GL_ONE, GL_ZERO);
glBlendEquation(GL_FUNC_ADD);

Normal Alpha-Blending
---------------------
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glBlendEquation(GL_FUNC_ADD);

Additiv (with Alpha)
--------------------
glBlendFunc(GL_SRC_ALPHA, GL_ONE);
glBlendEquation(GL_FUNC_ADD);

Subtract
--------
glBlendFunc(GL_SRC_ALPHA, GL_ONE);
glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

Multiply
--------
glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
glBlendEquation(GL_FUNC_ADD);

Screen
------
glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
glBlendEquation(GL_FUNC_ADD);

