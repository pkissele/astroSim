#include <fstream>
#include <iostream>
#include <tuple>
#include <array>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <numeric>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <random>
#include <filesystem>
#include <cstdlib>


#include <Eigen/Dense>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>
#include <cairo.h>

using namespace std;

using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;
using Vec2 = Eigen::Vector2d;


struct Body {
    double mass;
    Vec2 pos;
    Vec2 vel;
};


const double G = 9.8; // Arbitrary for now
const double pi = 3.14159265;


void draw_frame(const std::vector<Body>& bodies, const std::string& filename) {
    int width = 800;
    int height = 600;
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t* cr = cairo_create(surface);

    // background
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // draw particles
    cairo_set_source_rgb(cr, 1,1,1);
    for (auto& p : bodies) {
        double px = (p.pos[0])/20 * width;  // map [0,20] -> [0,width]
        double py = height-((p.pos[1])/20 * height); // map [0,20] -> [0,height]
        cairo_arc(cr, px, py, 3, 0, 2*pi);
        cairo_fill(cr);
    }

    cairo_surface_write_to_png(surface, filename.c_str());
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}


int main() {
	const int N = 1;
	vector<Body> bodies(N);

	bodies[0] = Body{10, Vec2(2, 10), Vec2(3, 0)};
    // bodies[1] = Body{1, Vec2(5, 0), Vec2(-1, 0)};

    vector<vector<double>> extForces(1);
    extForces[0] = {0, -G};

	double t = 0;
    int endT = 5;
    double dt = 0.01;
    int nframes = (endT-t)/dt;

    draw_frame(bodies, "output/frame_" + std::to_string(0) + ".png");

    for (int frame=1; frame<nframes; ++frame) {
        double ti = t + dt;
		// cout << frame << endl;

        // update particles positions
        for (int i = 0; i < N; i++) {
            for (int f=0; f<extForces.size(); f++) {
                bodies[i].vel[0] += bodies[i].mass * extForces[f][0]*dt;
                bodies[i].vel[1] += bodies[i].mass * extForces[f][1]*dt;
            }
            bodies[i].pos[0] += bodies[i].vel[0]*dt;
            bodies[i].pos[1] += bodies[i].vel[1]*dt;

            if((bodies[i].pos[1] < 0) && (bodies[i].vel[1] < 0)) {
                cout << "flipped" << endl;
                bodies[i].vel[1] *= -1;
            }
		}

        // save frame
        draw_frame(bodies, "output/frame_" + std::to_string(frame) + ".png");
    }

    string pyComm = "python img2gif.py output " + to_string(nframes) + " " + to_string(dt*1000);

    system(pyComm.c_str());

	return 0;
}