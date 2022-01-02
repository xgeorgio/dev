SELECT 
    ((Sy * Sxx) - (Sx * Sxy)) / ((N * (Sxx)) - (Sx * Sx)) AS a,
    ((N * Sxy) - (Sx * Sy)) / ((N * Sxx) - (Sx * Sx)) AS b
    --((N * Sxy) - (Sx * Sy)) / SQRT( (((N * Sxx) - (Sx * Sx)) * ((N * Syy - (Sy * Sy)))) ) AS r
    FROM
      (
      SELECT SUM(timestamp/10e6) AS Sx, SUM(ph) AS Sy,
        SUM((timestamp/10e6) * (timestamp/10e6)) AS Sxx,
        SUM((timestamp/10e6) * ph) AS Sxy,
        SUM(ph * ph) AS Syy,
        COUNT(*) AS N
        FROM Quality
      ) linreqr;
	  