function [hval, ds, ee, dsPAlt ] = HavDistEarthPAlt( lat1, lon1, alt1, lat2, lon2, alt2 )
% Haversine formula for Earth-plus-Altitude distance calculations
% IN:  lat/lon/alt (deg, ft) of two points
% OUT: hval=ratio, ds=distance, ee=ds.error
% See: https://en.wikipedia.org/wiki/Haversine_formula

% Earth's radius: 6356.752 km at the poles to 6378.137 km at the equator
EC=(6356.752+6378.137)/2;

% convert altitudes from ft to km
malt1=alt1*(2.54*12/100)/1000;
malt2=alt2*(2.54*12/100)/1000;     

% add to Earth's radius (Lo/Hi values)
ECpaltLo = EC + min([malt1 malt2]);
ECpaltHi = EC + max([malt1 malt2]);

% calculate the two spherical distances
[hval0,ds0,ee0]=HavF(ECpaltLo,lat1,lon1,lat2,lon2);
[hvalA,dsA,eeA]=HavF(ECpaltHi,lat1,lon1,lat2,lon2);

% 3-D distance estimation via trapezoid diagonal
dsPAlt = sqrt( (malt1-malt2)^2 + (ds0+(dsA-ds0)/2)^2 );

% return all intermediate values
hval=[hval0; hvalA];
ds=[ds0; dsA];
ee=[ee0; eeA];

end
