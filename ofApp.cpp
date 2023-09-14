#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(25);
	ofSetWindowTitle("openframeworks");

	ofBackground(239);
	ofEnableDepthTest();

	this->line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
	this->sphere_list.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}

//--------------------------------------------------------------
void ofApp::update() {

	this->face.clear();
	this->line.clear();

	float phi_deg_step = 1.5;
	float theta_deg_step = 6;

	float R = 250;
	float r = R * 0.25;
	float threshold = 0.4;

	for (float phi_deg = 0; phi_deg < 360; phi_deg += phi_deg_step) {

		for (float theta_deg = 0; theta_deg < 360; theta_deg += theta_deg_step) {

			auto noise_value = ofNoise(glm::vec4(this->make_point(R, r, theta_deg, phi_deg) * 0.035, ofGetFrameNum() * 0.02));
			if (noise_value < threshold) { continue; }

			auto noise_1 = ofNoise(glm::vec4(this->make_point(R, r, theta_deg - theta_deg_step, phi_deg) * 0.035, ofGetFrameNum() * 0.02));
			auto noise_2 = ofNoise(glm::vec4(this->make_point(R, r, theta_deg, phi_deg + phi_deg_step) * 0.035, ofGetFrameNum() * 0.02));
			auto noise_3 = ofNoise(glm::vec4(this->make_point(R, r, theta_deg, phi_deg - phi_deg_step) * 0.035, ofGetFrameNum() * 0.02));
			auto noise_4 = ofNoise(glm::vec4(this->make_point(R, r, theta_deg + theta_deg_step, phi_deg) * 0.035, ofGetFrameNum() * 0.02));

			auto index = this->face.getNumVertices();
			vector<glm::vec3> vertices;

			vertices.push_back(glm::vec3(this->make_point(R, r, theta_deg - theta_deg_step * 0.5, phi_deg - phi_deg_step * 0.5)));
			vertices.push_back(glm::vec3(this->make_point(R, r, theta_deg + theta_deg_step * 0.5, phi_deg - phi_deg_step * 0.5)));
			vertices.push_back(glm::vec3(this->make_point(R, r, theta_deg - theta_deg_step * 0.5, phi_deg + phi_deg_step * 0.5)));
			vertices.push_back(glm::vec3(this->make_point(R, r, theta_deg + theta_deg_step * 0.5, phi_deg + phi_deg_step * 0.5)));

			this->face.addVertices(vertices);

			this->face.addIndex(index + 0); this->face.addIndex(index + 1); this->face.addIndex(index + 3);
			this->face.addIndex(index + 0); this->face.addIndex(index + 3); this->face.addIndex(index + 2);

			if (noise_1 < threshold) {

				this->line.addVertex(vertices[0]);
				this->line.addVertex(vertices[2]);

				this->line.addIndex(this->line.getNumVertices() - 1);
				this->line.addIndex(this->line.getNumVertices() - 2);
			}

			if (noise_2 < threshold) {

				this->line.addVertex(vertices[2]);
				this->line.addVertex(vertices[3]);

				this->line.addIndex(this->line.getNumVertices() - 1);
				this->line.addIndex(this->line.getNumVertices() - 2);
			}

			if (noise_3 < threshold) {

				this->line.addVertex(vertices[0]);
				this->line.addVertex(vertices[1]);

				this->line.addIndex(this->line.getNumVertices() - 1);
				this->line.addIndex(this->line.getNumVertices() - 2);
			}

			if (noise_4 < threshold) {

				this->line.addVertex(vertices[1]);
				this->line.addVertex(vertices[3]);

				this->line.addIndex(this->line.getNumVertices() - 1);
				this->line.addIndex(this->line.getNumVertices() - 2);

			}
		}
	}

	ofSeedRandom(39);

	this->sphere_list.clear();

	vector<ofColor> base_color_list;
	ofColor color;
	vector<int> hex_list = { 0xef476f, 0xffd166, 0x06d6a0, 0x118ab2, 0x073b4c };
	for (auto hex : hex_list) {

		color.setHex(hex);
		base_color_list.push_back(color);
	}

	int color_index = 0;
	for (int i = 0; i < 400; i++) {

		auto u_speed = ofRandom(0.5, 2);
		auto v_speed = ofRandom(0.5, 2);
		auto sphere_u = ofRandom(360) + ofGetFrameNum() * u_speed;
		auto sphere_v = ofRandom(360) + ofGetFrameNum() * v_speed;
		int sphere_r = ofRandom(5, r * 0.8);
		
		this->sphere_list.addVertex(this->make_point(R, sphere_r, sphere_u, sphere_v));
		this->sphere_list.addColor(base_color_list[color_index]);
		color_index = (color_index + 1) % base_color_list.size();
	}

	for (int i = 0; i < this->sphere_list.getNumVertices(); i++) {

		for (int k = i + 1; k < this->sphere_list.getNumVertices(); k++) {

			if (glm::distance(this->sphere_list.getVertex(i), this->sphere_list.getVertex(k)) < r * 0.35) {

				this->sphere_list.addIndex(i);	this->sphere_list.addIndex(k);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	int R = 250;
	int r = R * 0.25;

	this->cam.begin();
	ofRotateX(90);
	this->cam.setPosition(R, 0, r * 2);

	ofSetLineWidth(3);
	ofSetColor(0);
	this->line.draw();

	ofSetColor(255);
	this->face.draw();

	ofSetLineWidth(4.5);
	this->sphere_list.drawWireframe();
	for (int i = 0; i < this->sphere_list.getNumVertices(); i++) {

		ofSetColor(this->sphere_list.getColor(i));
		ofDrawSphere(this->sphere_list.getVertex(i), 2);
	}

	this->cam.end();
}

//--------------------------------------------------------------
glm::vec3 ofApp::make_point(float R, float r, float u, float v, float scale) {

	// 数学デッサン教室 描いて楽しむ数学たち　P.31

	u *= DEG_TO_RAD;
	v *= DEG_TO_RAD;

	auto x = (R + r * cos(u) * scale) * cos(v);
	auto y = (R + r * cos(u) * scale) * sin(v);
	auto z = r * sin(u) * scale;

	return glm::vec3(x, y, z);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}