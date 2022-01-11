Title:<br/>
<b>Pegasus: Java-based Protein Prediction Web service.</b>

Description:<br/>
<p>'Pegasus' is a package for protein folding prediction implemented in early versions of Java (1.0-1.1) back in the early '90s. It is network-based service using standard TCP/IP sockets, compressed I/O streaming, custom priority queue for requests processing, mail messaging and multi-threaded evaluation of the back-end NN-MLP (pre-trained). The package itself includes a web-based client app (Java applet), a server app (CLI) and a server manager app (CLI).</p>
<p>Unfortunately Java has now depricated the applet API, hence the web-based client app is now unable to execute in modern web browsers. However, with only few modifications, it should be able to run as stand-alone client app with any recent Java JDK. For details about the design, implementation and configuration, see the extensive reference guide in the 'doc' folder.</p>
