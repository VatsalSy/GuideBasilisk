#include "navier-stokes/centered.h"
#include "two-phase.h"
#include "tension.h"
#include "tag.h"
#include "view.h"


/**
We define the radius of the jet, the initial jet length, the Reynolds
number and the surface tension coefficient. */

#define radius 1./12.
#define length 0.025
#define Re 5800
#define SIGMA 3e-5

/**
The default maximum level of refinement is 10 and the error threshold
on velocity is 0.1. */

int maxlevel = 7;
double uemax = 0.1;

scalar f0[];
//Initial conditions
event init (t = 0) {
  if (!restore (file = "dump")) {
    refine (x < 1.2*length && sq(y) + sq(z) < 2.*sq(radius) && level < maxlevel);
    fraction (f0, sq(radius) - sq(y) - sq(z));
    f0.refine = f0.prolongation = fraction_refine;
    restriction ({f0});
    foreach() {
      f[] = f0[]*(x < length);
      u.x[] = f[];
    }
    boundary ({f,u.x});
  }
}


/**
To impose boundary conditions on a disk we use an auxilliary volume
fraction field *f0* which is one inside the cylinder and zero
outside. We then set an oscillating inflow velocity on the
left-hand-side and free outflow on the right-hand-side. */
u.n[left]  = dirichlet(f0[]*(1. + 0.05*sin (10.*2.*pi*t)));
u.t[left]  = dirichlet(0);
p[left]    = neumann(0);
f[left]    = f0[];

u.n[right] = neumann(0);
p[right]   = dirichlet(0);

event adapt (i++) {
  adapt_wavelet ({f,u}, (double[]){0.01,uemax,uemax,uemax}, maxlevel);
}
/**
The program can take two optional command-line arguments: the maximum
level and the error threshold on velocity. */
event end (t = 2.5) {
  printf ("i = %d t = %g\n", i, t);
}
int main ()
{
  /**
  The initial domain is discretised with $64^3$ grid points. We set
  the origin and domain size. */

  init_grid (64);
  origin (0, -1.5, -1.5);
  L0 = 3.;

  /**
  We set the density and viscosity of each phase as well as the
  surface tension coefficient and start the simulation. */

  rho1 = 1., rho2 = 1./27.84;
  mu1 = 2.*radius/Re*rho1, mu2 = 2.*radius/Re*rho2;
  f.sigma = SIGMA;

  run();
}

event logWriting (i++) {
  static FILE * fp;
  if (i == 0) {
    fprintf (ferr, "i dt t\n");
    fp = fopen ("log", "w");
    fprintf (fp, "i dt t\n");
    fprintf (fp, "%d %g %g\n", i, dt, t);
    fclose(fp);
  } else {
    fp = fopen ("log", "a");
    fprintf (fp, "%d %g %g\n", i, dt, t);
    fclose(fp);
  }
  fprintf (ferr, "%d %g %g\n", i, dt, t);
}

/**
We generate an animation using Basilisk View. */
event movie (t += 1e-2)
{
#if dimension == 2
  scalar omega[];
  vorticity (u, omega);
  view (tx = -0.5);
  clear();
  draw_vof ("f");
  squares ("omega", linear = true, spread = 10);
  box ();
#else // 3D
  scalar pid[];
  foreach()
    pid[] = fmod(pid()*(npe() + 37), npe());
  boundary ({pid}); // not used for the moment
  view (camera = "iso",
	fov = 14.5, tx = -0.418, ty = 0.288,
	width = 1600, height = 1200);
  clear();
  draw_vof ("f");
  box ();
#endif // 3D
  static FILE * fp = popen ("ppm2mp4 > movie.mp4", "w");
  save (fp = fp);
}

event snapshot (t = 0; t += 0.1)
{
  dump (file = "dump");
  char nameGfs[80];
  sprintf (nameGfs, "intermediateGfs/snapshot-%2.1f.gfs", t);
  output_gfs (file = nameGfs, t = t);
  char nameBview[80];
  sprintf (nameBview, "intermediateBview/snapshot-%2.1f", t);
  dump (file = nameBview);
}
event gfsview (i++) {
  static FILE * fp = popen ("gfsview3D", "w");
  output_gfs (fp);
}




/**
We save snapshots of the simulation at regular intervals to
restart or to post-process with [bview](src/bview). */
/*event snapshot (t = 0; t += 0.1) {
  char name[80];
  sprintf (name, "intermediate/snapshot-%g", t);
  scalar pid[];
  foreach()
    pid[] = fmod(pid()*(npe() + 37), npe());
  boundary ({pid});
  dump (name);
}*/
