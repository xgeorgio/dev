SELECT 
    SUM( (ph-(SELECT AVG(ph) FROM Quality)) * (ph-(SELECT AVG(ph) FROM Quality)) ) / (COUNT(ph)-1) AS var
FROM Quality;
