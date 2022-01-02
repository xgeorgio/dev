SELECT 
SELECT Vcount, Vmin, Vmax, Vavg, Vvar, a, b, rsq FROM
(SELECT 
    COUNT(ph) AS Vcount,
    MIN(ph) AS Vmin,
    MAX(ph) AS Vmax,
    AVG(ph) AS Vavg,
    SUM( (ph-(SELECT AVG(ph) FROM Quality)) * (ph-(SELECT AVG(ph) FROM Quality)) ) / (COUNT(ph)-1) AS Vvar
    FROM Quality 
    AS tbl1) 
JOIN
(SELECT 
    ((Sy * Sxx) - (Sx * Sxy)) / ((N * (Sxx)) - (Sx * Sx)) AS a,
    ((N * Sxy) - (Sx * Sy)) / ((N * Sxx) - (Sx * Sx)) AS b,
    -- ((N * Sxy) - (Sx * Sy)) / SQRT( (((N * Sxx) - (Sx * Sx)) * ((N * Syy - (Sy * Sy)))) ) AS r
    ((N * Sxy) - (Sx * Sy)) * ((N * Sxy) - (Sx * Sy)) / (((N * Sxx) - (Sx * Sx)) * ((N * Syy - (Sy * Sy)))) AS rsq
    FROM
      (
      SELECT SUM(timestamp/10e6) AS Sx, SUM(ph) AS Sy,
        SUM((timestamp/10e6) * (timestamp/10e6)) AS Sxx,
        SUM((timestamp/10e6) * ph) AS Sxy,
        SUM(ph * ph) AS Syy,
        COUNT(*) AS N
        FROM Quality
      )
    AS tbl2);
