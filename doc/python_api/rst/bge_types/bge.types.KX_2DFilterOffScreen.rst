KX_2DFilterOffScreen(CValue)
============================

.. module:: bge.types

base class --- :class:`CValue`

.. class:: KX_2DFilterOffScreen(CValue)

   2D filter custom off screen.

   .. attribute:: width

      The off screen width, -1 if the off screen can be resized dynamically (read-only).

      :type: integer

   .. attribute:: height

      The off screen height, -1 if the off screen can be resized dynamically (read-only).

      :type: integer

   .. attribute:: colorBindCodes

      The bind code of the color textures attached to the off screen (read-only).

      .. warning:: If the off screen can be resized dynamically (:data:`width` of :data:`height` equal to -1), the bind codes may change.

      :type: list of 8 integers

   .. attribute:: depthBindCode

      The bind code of the depth texture attached to the off screen (read-only).

      .. warning:: If the off screen can be resized dynamically (:data:`width` of :data:`height` equal to -1), the bind code may change.

      :type: integer
