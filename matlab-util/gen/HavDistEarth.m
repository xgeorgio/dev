function [hval, ds, ee] = HavDistEarth( lat1, lon1, lat2, lon2 )
% Haversine formula for Earth-surface distance calculations
% IN:  lat/lon of two points
% OUT: hval=ratio, ds=distance, ee=ds.error
% See: https://en.wikipedia.org/wiki/Haversine_formula

% Earth's radius: 6356.752 km at the poles to 6378.137 km at the equator
EC=(6356.752+6378.137)/2;
[hval,ds,ee]=HavF(EC,lat1,lon1,lat2,lon2);

end
