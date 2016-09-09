#include <readtet.h>
#include <geopick/pick2d.h>
#include <igl/writeOBJ.h>
#include <iostream>
#include <string>

using std::endl;
using std::string;

void usage()
{
	std::cerr <<
R"zzz(This program translate tetgen output files into an Wavefront OBJ file
Usage: -i <prefix> -o <file>
	-i prefix: tet file prefix
	-o file: OBJ file
NOTE: This program only prints faces present in the .faces file, usually it only contains boundary faces.
)zzz";
}

int main(int argc, char* argv[])
{
	string iprefix, ofn;
	int opt;
	while ((opt = getopt(argc, argv, "i:o:")) != -1) {
		switch (opt) {
			case 'i': 
				iprefix = optarg;
				break;
			case 'o':
				ofn = optarg;
				break;
			default:
				std::cerr << "Unrecognized option: " << optarg << endl;
				usage();
				return -1;
		}
	}
	if (iprefix.empty() || ofn.empty()) {
		std::cerr << "Missing options" << std::endl;
		usage();
		return -1;
	}
	Eigen::MatrixXd V;
	Eigen::MatrixXi E;
	Eigen::MatrixXi F;
	Eigen::MatrixXi P;
	Eigen::VectorXi EBM;
	Eigen::VectorXi FBM;
	try {
		readtet(iprefix, V, E, P, &EBM);
		readtet_face(iprefix, F, &FBM);
		Eigen::MatrixXd outV;
		Eigen::MatrixXi outF;
		geopick(V, {F}, outV, outF);
		igl::writeOBJ(ofn, outV, outF);
	} catch (std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
}