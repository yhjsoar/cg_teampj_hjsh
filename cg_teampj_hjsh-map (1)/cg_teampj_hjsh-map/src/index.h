#pragma once
#ifndef __INDEX_H__
#define __INDEX_H__



std::vector<vertex> create_cube_vertices() {
	std::vector<vertex> v;
	float x[] = { 0.5, 0.5, -0.5, -0.5 };
	float y[] = { -0.5, 0.5, 0.5, -0.5 };
	float y2[] = { -0.5, 0.5, 0.5, -0.5 };
	float z2[] = { -0.5, -0.5, 0.5, 0.5 };
	float x3[] = { 0.5, -0.5, -0.5, 0.5 };
	float z3[] = { -0.5, -0.5, 0.5, 0.5 };
	int i;
	//up
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(x[i], y[i], 0.5),
			vec3(0, 0, 1),
			vec2(1, -1)
			});
	}


	//front
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(0.5, y2[i], z2[i]),
			vec3(1, 0, 0),
			vec2(1, 1)
			});
	}

	//down
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(z2[i], y2[i], -0.5),
			vec3(0, 0, -1),
			vec2(-1, 1)
			});
	}

	//back
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(-0.5, y2[i], -z2[i]),
			vec3(-1, 0, 0),
			vec2(-1, -1)
			});
	}

	//left
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(x3[i], 0.5, z3[i]),
			vec3(0, 1, 0),
			vec2(1, 0)
			});
	}

	//right
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(-x3[i], -0.5, z3[i]),
			vec3(0, -1, 0),
			vec2(0, 1)
			});
	}
	
	return v;
}

std::vector<vertex> create_character_vertices()
{
	std::vector<vertex> v;
	//HEAD
	v.push_back({ vec3(0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(0.5,(float)1.8,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,(float)1.8,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,(float)1.8,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(-0.5,(float)1.8,0),vec3(1.0,1.0,0),vec2(1,1) });

	//BODY
	v.push_back({ vec3(0.5,0.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,0.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,0.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,0.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });

	//RIGHT LEG
	v.push_back({ vec3((float)0.3,(float)-0.4,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)-0.4,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)-0.4,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)-0.4,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });

	//LEFT LEG
	v.push_back({ vec3((float)-0.2,(float)-0.4,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)-0.4,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2,(float)-0.4,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)-0.4,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });

	//RIGHT FOOT
	v.push_back({ vec3((float)0.4,(float)-0.5,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.1,(float)-0.5,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.4,(float)-0.5,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.1,(float)-0.5,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.4,(float)-0.4,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.1,(float)-0.4,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.4,(float)-0.4,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.1,(float)-0.4,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });

	//LEFT FOOT
	v.push_back({ vec3((float)-0.1,(float)-0.5,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.4,(float)-0.5,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.1,(float)-0.5,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.4,(float)-0.5,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.1,(float)-0.4,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.4,(float)-0.4,(float)0.6),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.1,(float)-0.4,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.4,(float)-0.4,(float)0.2),vec3(1.0,0.5,0.0),vec2(1,0) });

	//MOUTH
	v.push_back({ vec3((float)0.3,(float)1.2,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.2,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.2,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.2,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.3,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.3,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.3,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.3,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });

	//RIGHT WING
	v.push_back({ vec3((float)0.6,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });

	//LEFT WING
	v.push_back({ vec3((float)-0.5,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });



	//RIGHT EYE
	v.push_back({ vec3((float)0.55,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });

	//LEFT EYE
	v.push_back({ vec3((float)-0.5,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	return v;
}

std::vector<vertex> create_character_vertices_right_front()
{
	std::vector<vertex> v;
	//HEAD
	v.push_back({ vec3(0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(0.5,(float)1.8,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,(float)1.8,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,(float)1.8,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(-0.5,(float)1.8,0),vec3(1.0,1.0,0),vec2(1,1) });

	//BODY
	v.push_back({ vec3(0.5,0.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,0.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,0.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,0.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });

	//RIGHT LEG
	v.push_back({ vec3((float)0.3, -0.175f * sqrtf(3),(float)0.225),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2, -0.175f * sqrtf(3),(float)0.225),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3, -0.2f * sqrtf(3), (float)0.1),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2, -0.2f * sqrtf(3), (float)0.1),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });

	//LEFT LEG
	v.push_back({ vec3((float)-0.2, -0.175f * sqrtf(3),(float)0.575),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3, -0.175f * sqrtf(3),(float)0.575),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2, -0.2f * sqrtf(3), (float)0.5),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3, -0.2f * sqrtf(3), (float)0.5),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });

	//RIGHT FOOT
	v.push_back({ vec3((float)0.4, -0.225f * sqrtf(3) - 0.15f,0.05f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)0.1, -0.225f * sqrtf(3) - 0.15f,0.05f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)0.4, -0.225f * sqrtf(3) + 0.05f,0.05f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)0.1, -0.225f * sqrtf(3) + 0.05f,0.05f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)0.4, -0.175f * sqrtf(3) - 0.15f,0.1f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)0.1, -0.175f * sqrtf(3) - 0.15f,0.1f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)0.4, -0.175f * sqrtf(3) + 0.05f,0.1f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)0.1, -0.175f * sqrtf(3) + 0.05f,0.1f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });

	//LEFT FOOT
	v.push_back({ vec3((float)-0.1, -0.25f * sqrtf(3) + 0.15f, 0.55f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)-0.4, -0.25f * sqrtf(3) + 0.15f, 0.55f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)-0.1, -0.25f * sqrtf(3) - 0.05f, 0.55f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)-0.4, -0.25f * sqrtf(3) - 0.05f, 0.55f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)-0.1, -0.2f * sqrtf(3) + 0.15f, 0.5f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)-0.4, -0.2f * sqrtf(3) + 0.15f, 0.5f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)-0.1, -0.2f * sqrtf(3) - 0.05f, 0.5f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)-0.4, -0.2f * sqrtf(3) - 0.05f, 0.5f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });

	//MOUTH
	v.push_back({ vec3((float)0.3,(float)1.2,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.2,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.2,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.2,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.3,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.3,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.3,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.3,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });

	//RIGHT WING
	v.push_back({ vec3((float)0.6,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });

	//LEFT WING
	v.push_back({ vec3((float)-0.5,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });

	//RIGHT EYE
	v.push_back({ vec3((float)0.55,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });

	//LEFT EYE
	v.push_back({ vec3((float)-0.5,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	return v;
}

std::vector<vertex> create_character_vertices_left_front()
{
	std::vector<vertex> v;
	//HEAD
	v.push_back({ vec3(0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(0.5,(float)1.8,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,(float)1.8,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,(float)1.8,0),vec3(1.0,1.0,0),vec2(1,1) });
	v.push_back({ vec3(-0.5,(float)1.8,0),vec3(1.0,1.0,0),vec2(1,1) });

	//BODY
	v.push_back({ vec3(0.5,0.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,0.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,0.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,0.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(0.5,1.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });
	v.push_back({ vec3(-0.5,1.0,-1.0),vec3(1.0,1.0,0.0),vec2(1,1) });

	//RIGHT LEG
	v.push_back({vec3((float)0.3, -0.175f*sqrtf(3),(float)0.575),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({vec3((float)0.2, -0.175f*sqrtf(3),(float)0.575),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({vec3((float)0.3, -0.2f*sqrtf(3), (float)0.5),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({vec3((float)0.2, -0.2f*sqrtf(3), (float)0.5),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.2,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	

	//LEFT LEG
	v.push_back({ vec3((float)-0.2, -0.175f * sqrtf(3),(float)0.225),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({vec3((float)-0.3, -0.175f*sqrtf(3),(float)0.225),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({vec3((float)-0.2, -0.2f*sqrtf(3), (float)0.1),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({vec3((float)-0.3, -0.2f*sqrtf(3), (float)0.1),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)0.0,(float)0.4),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.2,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)0.0,(float)0.3),vec3(1.0,0.5,0.0),vec2(1,0) });

	//RIGHT FOOT
	v.push_back({ vec3((float)0.4, -0.25f * sqrtf(3) + 0.15f, 0.55f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)0.1, -0.25f * sqrtf(3) + 0.15f, 0.55f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)0.4, -0.25f * sqrtf(3) - 0.05f, 0.55f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)0.1, -0.25f * sqrtf(3) - 0.05f, 0.55f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)0.4, -0.2f * sqrtf(3) + 0.15f, 0.5f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)0.1, -0.2f * sqrtf(3) + 0.15f, 0.5f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)0.4, -0.2f * sqrtf(3) - 0.05f, 0.5f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });
	v.push_back({ vec3((float)0.1, -0.2f * sqrtf(3) - 0.05f, 0.5f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1,0) });

	//LEFT FOOT
	v.push_back({ vec3((float)-0.4, -0.225f * sqrtf(3) - 0.15f,0.05f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)-0.1, -0.225f * sqrtf(3) - 0.15f,0.05f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)-0.4, -0.225f * sqrtf(3) + 0.05f,0.05f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)-0.1, -0.225f * sqrtf(3) + 0.05f,0.05f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)-0.4, -0.175f * sqrtf(3) - 0.15f,0.1f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)-0.1, -0.175f * sqrtf(3) - 0.15f,0.1f + 0.15f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)-0.4, -0.175f * sqrtf(3) + 0.05f,0.1f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });
	v.push_back({ vec3((float)-0.1, -0.175f * sqrtf(3) + 0.05f,0.1f - 0.05f * sqrtf(3)), vec3(1.0, 0.5, 0.0), vec2(1, 0) });

	//MOUTH
	v.push_back({ vec3((float)0.3,(float)1.2,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.2,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.2,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.2,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.3,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.3,(float)1.3),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)0.3,(float)1.3,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });
	v.push_back({ vec3((float)-0.3,(float)1.3,(float)1.0),vec3(1.0,0.5,0.0),vec2(1,0) });

	//RIGHT WING
	v.push_back({ vec3((float)0.6,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.6,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)0.5,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });

	//LEFT WING
	v.push_back({ vec3((float)-0.5,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)0.2,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)0.2,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)1.0,(float)0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.5,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });
	v.push_back({ vec3((float)-0.6,(float)1.0,(float)-0.7),vec3(1.0,0.5,0.0),vec2(1,1) });

	//RIGHT EYE
	v.push_back({ vec3((float)0.55,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.55,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)0.5,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });

	//LEFT EYE
	v.push_back({ vec3((float)-0.5,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.3,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.3,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.4,(float)0.4),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.5,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	v.push_back({ vec3((float)-0.55,(float)1.4,(float)0.3),vec3(1.0,1.0,0.0),vec2(0,0) });
	return v;
}


#endif