#include <chrono>
#include <cstdio>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"


vec3 color(const ray& r, hitable *world, int depth){
	hit_record rec;
	if (world->hit(r, 0.001, MAXFLOAT, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		} else {
			return vec3(0, 0, 0);
		}
	} else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y()+1.0); // map y from [-1,1] to [0,1]
		return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5, 0.7, 1.0); // linear interpolation
	}
}

int main(){
	auto timeStart = std::chrono::high_resolution_clock::now();

	int nx = 400;
	int ny = 200;
	int ns = 100;

	std::cout << "P3\n" << nx << " " << ny << "\n255\n";


	hitable *list[5];
	list[0] = new sphere(vec3(0,0,-1),0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
	list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
	list[3] = new sphere(vec3(-1,0,-1),0.5, new dielectric(1.5));
	list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));

	hitable *world = new hitable_list(list, 5);
	camera cam(vec3(-2,2,1),vec3(0,0,-1),vec3(0,1,0),90,float(nx)/float(ny));

	for(int j = ny - 1; j >= 0; j--){
		for (int i = 0; i < nx; i++){
			vec3 col(0, 0, 0);

			for (int s = 0; s < ns; s++) {
				float u = float(i + drand48()) / float(nx);
				float v = float(j + drand48()) / float(ny);
				ray r = cam.get_ray(u, v);
			  col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
	auto timeEnd = std::chrono::high_resolution_clock::now();
	auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
	fprintf(stderr, "\rDone: %.2f (sec)\n", passedTime / 1000);


}
