/*
	skull:
		stem 
			starts 0.8mm into cap bottom
			hole height = 60
			cyl height  = 44
			
			5.6 d
			
*/



// Row 4 decorated Cherry MX key

// dimensions Cherry MX connector
c_corr = .4;                // tolerance
c_horiz = 1.1;              // horizontal bar width
c_vert = 1.0;               // vertical bar width
c_dia = 4;                  // cross width
c_depth = 6;                // connector depth
c_space = 4;                // height of hollow inside
c_inset = .75;              // distance connector start to keycap base

// stuff
$fn = 64;

// construct the connector pin
module connector() 
{
	difference() {
		cylinder( h=c_space+c_corr, r=(c_dia+1+c_corr)/2 );
	    translate([0,0,c_depth/2]) union()
	    {
	        cube([c_vert+c_corr,c_dia+c_corr,c_depth+.1], center=true );
	        cube([c_dia+c_corr,c_horiz+c_corr,c_depth+.1], center=true );
	    }
	}

	//c_plate_w = ( base_w - 2*(dx*(z_front-c_depth)));
	//scale([0.9,0.9,1]) translate([0,0,c_depth-1]) cube([c_plate_w,c_plate_w,1], center=true);
}


// These seem fixed
base_w = 19;
top_y = 14;
dx = 2; 	// top sides are pushed to center
dy = 1;  // top is pushed back this far 

z_front = 4+2; // height at front edge
z_back  = 4+2+2;  // height at back edge

angle = atan((z_front-z_back)/top_y);
//echo ( "Angle=", angle);


/*
		   ______
		  /8   7/|
		 /5___6/	 |
		| |___|_ | 
		| /4  | 3/
	    |/1___2|/ 

		 
*/

module hull ()
{
	translate([-base_w/2, -base_w/2, 0]) {
	
	render() polyhedron (
		points = [
			[ 0, 0, 0 ], 
			[ 0		,0 		,0 ],
			[ base_w	,0 		,0 ],
			[ base_w	,base_w	,0 ],
			[ 0		,base_w	,0 ],
			[ dx			,dy 			,z_front ],
			[ base_w	-dx 	,dy 			,z_front ],
			[ base_w	-dx 	,dy+top_y	,z_back ],
			[ dx			,dy+top_y	,z_back ],
		], 
		triangles = [
			[1,2,6], [1,6,5], 
			[2,3,7], [2,7,6], 
			[3,4,8], [3,8,7], 
			[4,1,5], [4,5,8], 
			[5,6,7], [5,7,8], 
			[1,4,2], [2,4,3]
		]
	);
}
}

module stem() 
{
	difference(){
		cylinder(h=20, r=3);
		union()
		{
		}
	}
}


module cap ()
{
	difference()	 {
		hull();
		scale(0.9) {
			//translate ([0,0,0.21]) cube([base_w, base_w, 2]);
			difference(){
				hull();
				translate([0,0,c_depth-1.3+20/2]) cube([base_w,base_w,20], center=true);
			}
		}
	}
}


module cap_cyl()
{
$fn = 128;
r_cyl = 30;

translate( [10,10,0]){
rotate ([-angle,0,0]) 	difference()  {
      	rotate ([angle,0,0]) cap();
		rotate ([90,0,0]) translate([0,z_back+r_cyl-1.0 -1.2,-base_w/2]) cylinder(h=base_w, r=r_cyl);
	}
	connector();
	
}
}

translate([20,0,0]) cap();

cap_cyl();
