# visao

Um dos grandes desafios da robótica móvel e humanoide atualmente é aumentar as habilidades cognitivas destas máquinas autônomas, principalmente relacionadas com a interação dentro de ambientes e com objetos, animais e pessoas. Para desenvolver tais habilidades é fundamental incorporar sistemas de visão computacional que necessitam de estudos relacionados com calibração, correspondência e reconstrução  virtual de ambientes e objetos.
A problemática deste projeto está relacionada com a extensão funcional de um robô humanoide, que já se encontra em desenvolvimento no IFSC, através de um sistema de mapeamento e reconhecimento de objetos. Os objetivos estão relacionados com desenvolvimento de um sistema de visão artificial com mapeamento de posição considerando baixo custo bem com energeticamente eficiente.
O projeto será dividido em três eixos de desenvolvimento: estudo da visão computacional utilizando Matlab, estudo de requisito de hardware para implementação das funções introduzidas pelo Matlab em OpenCV e, finalmente, implementação dos algoritmos em um sistema embarcado.

Cronograma:

Leitura documentação
 - Dissertação Gunter e Edgar (./docs/)
 - Coversão do algoritmo sobel para OpenCV
 - Verificar:
   - http://www.vlfeat.org/index.html
   - http://www.petercorke.com/RVC/top/toolboxes/
   - http://mccormickml.com/2014/01/10/stereo-vision-tutorial-part-i/
   - http://www.peterkovesi.com/matlabfns/
   - http://www.petercorke.com/Machine_Vision_Toolbox.html
   - https://linuxconfig.org/introduction-to-computer-vision-with-opencv-on-linux
   - https://wiki.archlinux.org/index.php/User:Der_harm#Matlab
   - http://www.gnebehay.com/cmt/
   
Dicas Fábio:

Alternativa 1: Arquivos OCT 
https://www.gnu.org/software/octave/doc/v4.0.1/Getting-Started-with-Oct_002dFiles.html#Getting-Started-with-Oct_002dFiles

Alternativa 2: Arquivo MEX (pode ser utilizado tanto no MATLAB quanto no Octave e talvez SciLab - cabe verificação - ilib_mex_build)
https://www.gnu.org/software/octave/doc/v4.0.1/Mex_002dFiles.html#Mex_002dFiles

Alternativa 3: Shared-library - SCILAB

https://help.scilab.org/docs/6.0.0/en_US/section_eb691bc0a443eb3a95bc5db4d1be2102.html

Alternativa 4: REST-ful API (pode ser utilizado em conjunto com qualquer software praticamente)

http://libccv.org/doc/doc-http/

MATLAB, Octave: Um cliente usando algo como isso "https://github.com/fangq/jsonlab" + "urlread"
SciLab: http://forge.scilab.org/index.php/p/restclient/

Ou também existe a alternativa MATLAB/Octave/Scilab -> Java -> biblioteca (JNI - http://docs.oracle.com/javase/8/docs/technotes/guides/jni/ ou REST-ful - https://docs.oracle.com/cd/E24329_01/web.1211/e24983/client.htm#RESTF150)
http://www.mathworks.com/help/matlab/matlab_external/bringing-java-classes-and-methods-into-matlab-workspace.html
http://octave.sourceforge.net/java/overview.html
https://www.scilab.org/scilab/interoperability/connection_languages/java
