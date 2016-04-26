/* ----------------------------------------------------------------------
   PDPS - Particle Dynamics Parallel Simulator
   
   Copyright (2012) reserved by Lingqi Yang. 
   Email: ly2282@columbia.edu

   See the README file in the PDPS directory.
------------------------------------------------------------------------- */

#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "create_particle.h"
#include "neighbor.h"
#include "neigh_list.h"
#include "domain.h"
#include "error.h"
#include "fix_foam.h"
#include "particle.h"
#include "particle_type.h"
#include "region.h"
#include "random_park.h"
#include "update.h"
#include "group.h"
#include "update.h"
#include "compute.h"
#include "modify.h"

using namespace PDPS_NS;
using namespace FixConst;
enum{UP, DOWN, FRONT, BACK, LEFT, RIGHT};

/* ---------------------------------------------------------------------- */

FixFoam::FixFoam(PDPS *ps, int narg, char **arg) : Fix(ps, narg, arg)
{
	if (narg < 6) {
		error->all(FLERR,"Illegal fix Foam command");
	}
	
	region_flag = 0;
	neighbor_flag = level_flag = 0;
	int iarg;
	tid = atof(arg[3]);
	if (!strcmp(arg[4], "neighbor")) {
		neighbor_flag = 1;
	}
	else if (!strcmp(arg[4], "level")) {
		level_flag = 1;
	}
	int ngid = group->find_group(arg[5]);
	newgid = group->bitmask[ngid];

	ngid = group->find_group(arg[6]);
	refgid = group->bitmask[ngid];

}

/* ---------------------------------------------------------------------- */

FixFoam::~FixFoam()
{

}

/* ---------------------------------------------------------------------- */

int FixFoam::setmask()
{
	int mask = 0;
	mask |= POST_FORCE;
	return mask;
}

/* ---------------------------------------------------------------------- */

void FixFoam::init()
{

}

/* ---------------------------------------------------------------------- */

void FixFoam::setup()
{
	post_force();
}

/* ---------------------------------------------------------------------- */

void FixFoam::post_force()
{
	double **x = particle->x;
	double **f = particle->f;
	double **v = particle->v;
	double *mass = particle->mass;
	double *rmass = particle->rmass;
	double *radius = particle->radius;
	int rmass_flag = particle->rmass_flag;
	int *mask = particle->mask;
	int *bitmask = group->bitmask;
	int *type = particle->type;
	int nlocal = particle->nlocal;
	int *tag = particle->tag;
	double temp;
	CreateParticle createparticle(ps);
//	RanPark *random;
//	random = new RanPark(ps, seed);
//	class ParticleType *ptype = particle->ptype;
	int rised_flag = 0;

	if (level_flag == 1){
		for (int i = 0; i < nlocal; i++) {
			if (mask[i] & groupbit) {
				if (x[i][2] > modify->compute[0]->scalar){
					
					x[i][2] = domain->boxhi[2] + domain->xle;
					mask[i] = 1;
					mask[i] |= newgid;
					type[i] = tid;
					group->glocal[newgid] = group->glocal[newgid] + 1;
					group->gparticles[newgid] = group->gparticles[newgid] + 1;
					group->glocal[groupbit] = group->glocal[groupbit] - 1;
					group->gparticles[groupbit] = group->gparticles[groupbit] - 1;
					v[i][2] = 0.0;
				}
			}
		}
	}
	else if (neighbor_flag == 1){
		int *numneigh;
		numneigh = neighbor->neighlist->numneigh;
		for (int i = 0; i < nlocal; i++){
			if (mask[i] & groupbit) {
				if (numneigh[i] < 3){
					mask[i] = 1;
					mask[i] |= newgid;
					type[i] = tid;
					group->glocal[newgid] = group->glocal[newgid] + 1;
					group->gparticles[newgid] = group->gparticles[newgid] + 1;
					group->glocal[groupbit] = group->glocal[groupbit] - 1;
					group->gparticles[groupbit] = group->gparticles[groupbit] - 1;
					v[i][2] = 0.0;
				}
			}
		}
	}

	
}
