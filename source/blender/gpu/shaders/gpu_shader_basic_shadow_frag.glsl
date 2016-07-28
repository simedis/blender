#ifdef USE_VSM
varying vec4 v_position;
#endif

void main()
{
#ifdef USE_VSM
	/**
	 * This fragment shader was initially found at http://fabiensanglard.net/shadowmappingVSM/index.php
	 */

	float depth = v_position.z / v_position.w;
	depth = depth * 0.5 + 0.5;

	float moment1 = depth;
	float moment2 = depth * depth;

	// Adjusting moments using partial derivative
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25 * (dx * dx + dy * dy);

	gl_FragColor = vec4(moment1, moment2, 0.0, 0.0);
#endif

#ifdef USE_PENUMBRA
	gl_FragColor = vec4(gl_FragCoord.z / gl_FragCoord.w);
#endif
}
