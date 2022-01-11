function [hval, ds, ee] = HavF( r, lat1, lon1, lat2, lon2 )
% Haversine formula for spherical distance calculations
% IN:  r=radius (sphere), lat/lon (deg) of two points
% OUT: hval=ratio, ds=distance, ee=ds.error
% See: https://en.wikipedia.org/wiki/Haversine_formula

% Earth's radius: 6356.752 km at the poles to 6378.137 km at the equator
EC=(6356.752+6378.137)/2;

phi1=lat1/180*pi;  the1=lon1/180*pi;
phi2=lat2/180*pi;  the2=lon2/180*pi;

hval = (1-cos(phi2-phi1))/2 + cos(phi1)*cos(phi2)*(1-cos(the2-the1))/2;

ds = 2*r*asin(sqrt(hval));
ee = ds*0.5/100;

end

