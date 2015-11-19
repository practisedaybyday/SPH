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
#include "domain.h"
#include "error.h"
#include "fix_nuc.h"
#include "particle.h"
#include "particle_type.h"
#include "region.h"
#include "random_park.h"
#include "update.h"
#include "group.h"
#include "update.h"

using namespace PDPS_NS;
using namespace FixConst;
enum{UP, DOWN, FRONT, BACK, LEFT, RIGHT};

/* ---------------------------------------------------------------------- */

FixNuc::FixNuc(PDPS *ps, int narg, char **arg) : Fix(ps, narg, arg)
{
	if (narg < 9) {
		error->all(FLERR,"Illegal fix Nuc command");
	}
	
	region_flag = 0;

	int iarg;
	tid = atof(arg[3]);
	int ngid = group->find_group(arg[4]);
	newgid = group->bitmask[ngid];
	frequency = atof(arg[5]);
	gap = atof(arg[6]);
	iarg = 7;
	if (!strcmp(arg[iarg], "up"))
		direction = UP;
	else if (!strcmp(arg[iarg], "down"))
		direction = DOWN;
	else if (!strcmp(arg[iarg], "front"))
		direction = FRONT;
	else if (!strcmp(arg[iarg], "back"))
		direction = BACK;
	else if (!strcmp(arg[iarg], "left"))
		direction = LEFT;
	else if (!strcmp(arg[iarg], "right"))
		direction = RIGHT;
	else error->all(FLERR, "Illegal command option");
	seed = atof(arg[8]);
	count = 0;

}

/* ---------------------------------------------------------------------- */

FixNuc::~FixNuc()
{

}

/* ---------------------------------------------------------------------- */

int FixNuc::setmask()
{
	int mask = 0;
	mask |= POST_FORCE;
	return mask;
}

/* ---------------------------------------------------------------------- */

void FixNuc::init()
{

}

/* ---------------------------------------------------------------------- */

void FixNuc::setup()
{
	post_force();
}

/* ---------------------------------------------------------------------- */

void FixNuc::post_force()
{
	double **x = particle->x;
	double **f = particle->f;
	double *mass = particle->mass;
	double *rmass = particle->rmass;
	int rmass_flag = particle->rmass_flag;
	int *mask = particle->mask;
	int *bitmask = group->bitmask;
	int *type = particle->type;
	int nlocal = particle->nlocal;
	int *tag = particle->tag;
	double temp;
	CreateParticle createparticle(ps);
	RanPark *random;
	random = new RanPark(ps, seed);
//	class ParticleType *ptype = particle->ptype;
	int inside_flag;
	if (update->ntimestep % 10 == 0){
		for (int i = 0; i < nlocal; i++) {
			if (mask[i] & groupbit) {
				count++;
	//			temp = random->uniform();
				if (count > frequency){
					if (direction == UP)
						particle->ptype->create_particle(tid, x[i]);
					//createparticle.create_single(x[i][0], x[i][1], x[i][2] + gap);
					else if (direction == DOWN)
						createparticle.create_single(x[i][0], x[i][1], x[i][2] - gap);
					else if (direction == FRONT)
						createparticle.create_single(x[i][0] + gap, x[i][1], x[i][2]);
					else if (direction == BACK)
						createparticle.create_single(x[i][0] - gap, x[i][1], x[i][2]);
					else if (direction == LEFT)
						createparticle.create_single(x[i][0], x[i][1] - gap, x[i][2]);
					else if (direction == RIGHT)
						createparticle.create_single(x[i][0], x[i][1] + gap, x[i][2]);
					mask[particle->nlocal-1] |= newgid;
					tag[particle->nlocal-1] = particle->nlocal;
					group->glocal[newgid] = group->glocal[newgid] + 1;
					group->gparticles[newgid] = group->gparticles[newgid] + 1;
					count = 0;
				}

			}
		}
	}

	
}
