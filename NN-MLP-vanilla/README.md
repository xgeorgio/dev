Title:<br/>
<b>NN-MLP-vanilla: Neural Network Multi-Layer Perceptron programs suite in 'vanilla' C/C++</b>

Description:<br/>
<p>Prototype package for implementing Multi-Layer Perceptron feed-forward Neural Networks and variants of the Back-propagation training algorithm, as well as a feed-forward testing program for already trained and stored instances. The Back-propagation trainer implements various options for momentum, activation functions (sigmoid, tanh), output function (standard, winner-takes-all), etc. There is also a feature extractor program for 1st- and 2nd-order texture analysis in images (US examples included), as well as a feature selection optimizer implementing standard T-test and subsequent exhaustive combinations search. There is also a helper tool for randomly shuffling text input files before the NN training, if necessary.</p>
<p>There are two versions for the main programs, one is for console (CLI) that is trully cross-platform portable and a MS-Windows version that is based on bare Window SDK, i.e., forward-compatible to all Windows versions after Windows XP or so. The entire package is based on an earlier set of console programs, originally developed in SunSPARC/Solaris platform back in the early '90s, while the testing suite is based on a later Windows-based implementation. All non-Windows code is implemented as ANSI-compatible 'vanilla' C and C++, i.e., no STL use or other requirements, thus they can be ported to any other platform with minimum problems (already tested in various Linux and OS/2 versions). Due to changing compiler versions, some warnings may appear (e.g. for string-to-charptr), but in general the CLI versions should compile without problem everywhere with any modern C/C++ suite.</p>
<ul>
  <li>SIFX: ...</li>
  <li>FCD: ...</li>
  <li>BPM: ...</li>
  <li>MLP: ...</li>
  <li>Shuffle: ...</li>
</ul>

Examples of client request, e.g. via web browser:
<pre>
...
</pre>

<p>Algorithms for Medical Image Analysis – Part I: Applications Programming (in Greek)</p>
<p>H. Georgiou. Book published by Arnaoutis Publ. (Oct. 2012). The book is the first part of a two-volume work on all the intermediate processing levels in Computer Aided Diagnosis (CAD) or Automated Computer Diagnosis (ACD), focused primarily on the programming and development of relevant applications. It also covers all the necessary background theory and algorithms for Image Analysis and Medical Imaging, as well as Pattern Recognition and Machine Learning. All the material covered in this book is complete and independent, focusing on the characterization of ultrasonic images of human liver for diagnostic purposes, employing a wide variety of methodologies of statistical image analysis and Machine Learning. The same material has been used for lectures and tutorials in graduate (BSc) and post-graduate (MSc) level courses taught by the author for many years, in relation to Artificial Intelligence and Digital Imaging, as well as programming algorithms for Pattern Recognition. The book is accompanied by full source code (ANSI C/C++) of more than 12.000 lines and complete description of the implemented full applications that are presented therein.</p>
<a href="http://www.ekdoseis-arnaoutis.gr/panepistimiaka/algorithmoi-analysis-iatrikis-eikonas-tomos-a-detail" target="_blank">ISBN: 978-960-9764-09-4</a> (<a href="http://www.ekdoseis-arnaoutis.gr" target="_blank">Arnaoutis Publ.</a>, Oct. 2012).
