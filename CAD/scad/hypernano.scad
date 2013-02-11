
use <mx-keycap-minimal.scad>

MXSpacing  = 20; // 19mm is standard, little more room seems to be comfy
offsetX=0.25;    // 1/4 staggering
MXcutout=13.65;  // 14mm nominal, but lets make it tight

/// @todo : frame not long enough to house teensy (~1mm)

module teensy20()
{
  s=2.54;
  pinlen = 8; // len, pcb+tin=3 extra;


  module pinhole()
  {
	translate([s/2,s/2,0.75]) 
	{
		color("Green")
		difference() 
    		{ 
			cube([s,s,1.5], center=true);
 	  		cylinder(h=2,r=0.7,center=true, $fn=20 );
    		}

    		color("Black") translate ([0,0,2.25]) {cube([s*1,s,3],center=true);}
    		color("Gold") translate ([0,0,pinlen/2]) {cylinder(h=pinlen+3,r=0.7,center=true, $fn=20 );}
	}
  }
	
  rotate([180,0,90]) translate([0, -7*s/2,1]) {
  
  	//cube ( [7*s,12*s,1.5] );
	  for ( x=[0:11] ) {
	    translate([x*s,0,0])
		pinhole();
	    translate([x*s,6*s,0])
		pinhole();
	  }
	  for ( y=[1:5] ) {
	    translate([11*s,y*s,0])
		pinhole();
	  }
	  translate([4*s,s,  0]) pinhole();
	  translate([4*s,5*s,0]) pinhole();
	
		// PCB
		color( [0,0.4,0,1] ) {
			translate([0,1*s,0]) cube( [4*s,5*s, 1.5] );
			translate([5*s,1*s,0]) cube( [6*s,5*s, 1.5] );
			translate([4*s,2*s,0]) cube( [1*s,3*s, 1.5] );
		}
		// USB
		color("silver")	
			translate ( [0-1, 2*s, 1.5]) cube ( [1+3*s,3*s,4 ],center=yes);
	}
}

module teensy20frame()
{

}

module MX()
{
	MXcutout(MXSpacing,MXSpacing);
}

module MXcutout(x,y)
{
  RidgeDepth = 2;   // depth of support ridge above and below plate
  RidgeWidth = 1.5; // space around cutout where switch sits


 
/*
top_x
___
   | - top_z
   |_____top_ridge
         |  1.5mm FIX
     ----+
     |  bot_ridge
     |bot_z
     |
-----+
bot_x

*/ 

  //x,y from external - typical 19mm, useful 20?!

  top_z=1;
  bot_z=3;
  z=top_z+bot_z+1.5;

  bot_ridge=1;
  top_ridge=2;

  d_render=0.1; // cut-out delta to cleanup F5 render, should not modify model.

  if(x) 
  {
    	assign(MXSpacing  = x)

{
	translate([0,0,0/*-1*/]) {
	  	difference()
		{
			//whole cube:
			cube([x,y,z], center=true);
			// minus 14x14mm
			cube([MXcutout,MXcutout,z+d_render], center=true);
			translate ( [0,0,(bot_z-z)/2-d_render] ) cube([MXcutout+bot_ridge*2,MXcutout+bot_ridge*2,bot_z+2*d_render], center=true);
			translate ( [0,0,(z-top_z)/2+d_render] ) cube([MXcutout+top_ridge*2,MXcutout+top_ridge*2,top_z+2*d_render], center=true);
		}
	}
}
 
	// bottom of keycap stem is 6.6mm above top cut-out plane
	// cap_cyl();
	translate([0,0,(z-top_z)/2 +6.6 ]) color("blue",0.1) #cap();

/*	
	// supporting ridges
	difference() {
		cube(size = [x,y, 1.5 + 2*RidgeDepth ], center=true);		
		cube(size = [w+2*RidgeWidth,w+2*RidgeWidth, 20], center=true);
	}

	// internal cutout 14x14mm in 1.5mm plate
	difference() {
		cube(size = [x,y, 1.5  ], center=true);		
		cube(size = [w,w,20], center=true);
	}
*/
}
	// Cap dummies
    // color([0,0,0,0.1]) translate([0,0,6]) #cube( size = [w,w,20], center=true);
}

// ***********************************

if(MXSpacing < 19)
{
	echo ("\r*********\r   Original Cherry Keycaps will NOT fit\r*********");
}


// MXcutout();


// one half, 1/4 offset stagger.
module MXsymstagger()
{
  offsetX=0.25;
  for ( y = [0:3] ) {
    for ( x = [0:5] ) {
      translate( [x*MXSpacing+y*MXSpacing*offsetX,y*MXSpacing , 0] ) MX();
    }
  }  
}




// row of 5 with 1/4 offset stagger and left hand centered MX
module MXrow(offset)
{
	cnt=5;
  	translate( [offset*MXSpacing*offsetX/2,0, 0] ) MXcutout(MXSpacing+offset*MXSpacing*offsetX,MXSpacing);
	for ( x = [1:cnt-1] ) {
    		translate( [x*MXSpacing+offset*MXSpacing*offsetX,0 , 0] ) MX();
	}
}


// Complete 2x 5x4 matrix with center plane
module BlueCube()
{
	module 3mx(cnt)
	{
		for ( x = [0:cnt-1] ) {
    			translate( [0,x*MXSpacing,0] ) MX();
		}	
	}

	d=MXSpacing;
	translate([0*d, 0   *d,0]) { 3mx(3); }	
	translate([1*d, 0   *d,0]) { 3mx(3); }
	translate([2*d, 1   *d,0]) { 3mx(3); }
	translate([3*d, 1.25*d,0]) { 3mx(3); }
	translate([4*d, 0.75*d,0]) { 3mx(3); }
	translate([5*d, 0.75*d,0]) { 3mx(3); }
	translate([6*d, -.75*d,0]) { 3mx(2); }
	rotate([0,0,270]) translate([0.25*d, 2*d,0]) { 3mx(4); }

	color("red") translate([1.5*d, 0.25*d,1.25]) { cube([1*d,d/4,1.5]); }
	color("red") translate([2.5*d, 0.25*d,1.25]) { cube([1*d,d/2,1.5]); }

	/**** could do more: 
		* - AVR enclosure, fill gaps with keys
		* - MCP23018 + audio plug
		* - more keys
		* - hinges
		* - rotation
 	*/
	//if( enhanced) 
	{
		color("blue")
		{
				translate([0*d, 3   *d,0]) { 3mx(1); }	
				translate([1*d, 3   *d,0]) { 3mx(1); }	
				translate([6*d, 1.25*d,0]) { 3mx(2); }
		}
	}

}



// Complete 2x 5x4 matrix with center plane
module MXminimal2()
{
	translate([-5.5*MXSpacing,0.5*MXSpacing,-2.75]) 
	{
		// 4 row with center padding at switch level
		translate([0, 0*MXSpacing,0]) MXrow(0);
		translate([5*MXSpacing,0*MXSpacing,0]) MX();
		
		translate([0, 1*MXSpacing,0]) MXrow(0);
		translate([5*MXSpacing,1*MXSpacing,0]) MX();
		
		//translate([(5-0.5+0*offsetX)*MXSpacing, MXSpacing/2, 1.5]) cube( [MXSpacing*offsetX*2,MXSpacing, 1.25] );
		translate([0, 2*MXSpacing,0]) MXrow(1);
		
		translate([(5-0.5+1*offsetX)*MXSpacing, MXSpacing*1.5, 1.5]) cube( [MXSpacing*offsetX,MXSpacing, 1.25] );
		translate([0, 3*MXSpacing,0]) MXrow(2);
		
		// closed center:

	}
	translate ([0,2*MXSpacing,-1.25]) cube( [ 0.5*MXSpacing,2*MXSpacing, 1.25]) ; 
	

	//USB cutout
	translate ([-MXSpacing/2, 4*MXSpacing-1,-5.5]) {
		difference(){
			cube( [ 0.5*MXSpacing, 1.0, 2*2+1.5]) ;
			translate ([MXSpacing/2-4/2+0.01,0,0]) cube( [ 4,3,5],center=true) ; 
		}
	}	
}

union() {
	// move STL out
	translate( [125,5,10] ) {
		//MXcutout(30,20);
		MXminimal2();
		mirror([1,0,0]) MXminimal2();


		//   #render() import("MX.stl");



		//translate ([0,3*MXSpacing,-0.5]) cube( [ 1*MXSpacing,2*MXSpacing,1.0],center=true)  #teensy20();

		translate ([0,4*MXSpacing-1,0]) rotate ([0, 0, 180])  
		{
			//#teensy20();
		  	translate ([0,16,-3]) 	difference() 
			{
		     	cube([2+7*2.54,1+12*2.54,5],center=true);
		     	translate([0,-1,0]) cube([  7*2.54,  12*2.54+1,5.1],center=true);
			}
		}
	}
}


union() {
	// move STL out
	translate( [125,120 ,0] ) {

		BlueCube();

	}
}




/*
// Complete 2x 5x4 matrix with center plane
module MXminimal()
{
// 4 row with center padding at switch level
translate([0, 0*MXSpacing,0]) MXrow(1);
translate([(5-0.5+1*offsetX)*MXSpacing, -MXSpacing/2, 1.5]) cube( [MXSpacing*offsetX,MXSpacing, 1.25] );
translate([(5-0.5+1*offsetX)*MXSpacing, -MXSpacing/2, -2.75]) cube( [MXSpacing*offsetX,1.5, 5.5] );

translate([0, 1*MXSpacing,0]) MXrow(0);
translate([(5-0.5+0*offsetX)*MXSpacing, MXSpacing/2, 1.5]) cube( [MXSpacing*offsetX*2,MXSpacing, 1.25] );
translate([0, 2*MXSpacing,0]) MXrow(1);
translate([(5-0.5+1*offsetX)*MXSpacing, MXSpacing*1.5, 1.5]) cube( [MXSpacing*offsetX,MXSpacing, 1.25] );
translate([0, 3*MXSpacing,0]) MXrow(2);
}


MXminimal();
translate([10*MXSpacing,0,0]) mirror([1,0,0]) MXminimal();
*/
/*
// 4 row with center padding at switch level
translate([0, 0*MXSpacing,0]) MXrow(1);
translate([(5-0.5+1*offsetX)*MXSpacing, -MXSpacing/2, 1.5]) cube( [MXSpacing*offsetX,MXSpacing, 1.25] );
translate([(5-0.5+1*offsetX)*MXSpacing, -MXSpacing/2, -2.75]) cube( [MXSpacing*offsetX,1.5, 5.5] );

translate([0, 1*MXSpacing,0]) MXrow(0);
translate([(5-0.5+0*offsetX)*MXSpacing, MXSpacing/2, 1.5]) cube( [MXSpacing*offsetX*2,MXSpacing, 1.25] );
translate([0, 2*MXSpacing,0]) MXrow(1);
translate([(5-0.5+1*offsetX)*MXSpacing, MXSpacing*1.5, 1.5]) cube( [MXSpacing*offsetX,MXSpacing, 1.25] );
translate([0, 3*MXSpacing,0]) MXrow(2);
*/

/*

for ( y = [0:2] ) {
  translate( [y*MXSpacing*offsetX/2,y*MXSpacing , 0] ) MXcutout(MXSpacing+y*MXSpacing*offsetX,MXSpacing);
  for ( x = [1:5] ) {
    translate( [x*MXSpacing+y*MXSpacing*offsetX,y*MXSpacing , 0] ) MX();
  }
}
translate( [1*MXSpacing*offsetX/2,-1*MXSpacing , 0] ) MXcutout(MXSpacing+1*MXSpacing*offsetX,MXSpacing);
for ( x = [1:5] ) {
  translate( [x*MXSpacing+1*MXSpacing*offsetX,-1*MXSpacing , 0] ) MX();
}

*/

