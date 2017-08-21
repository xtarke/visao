%% Programa exemplo para detecção de objetos circulares
 % Fonte: http://www.mathworks.com/help/images/examples/detect-and-measure-circular-objects-in-an-image.html
    
%% Inicio do programa

% Faz a leitura da imagem (carrega) e mostra na tela
rgb = imread('test02Dir0.png');
figure
imshow(rgb)

% Utiliza a ferramenta para estimar o raio dos objetos a serem detectados
%d = imdistline;
% As peças do exemplo possuem raio em torno de 20-25 unidades (pixels?)

% Converte a imagem para uma imagem preto em branco e a mostra para
% perceber como as peças se comparam com o fundo
%gray_image = rgb2gray(rgb);
%imshow(gray_image);

% Por padrão a função que detecta os objetos precisa de que o objeto seja
% mais "brilhante" que o fundo, a opção 'ObjectPolarity' com 'dark' "resolve"
% a questão. Os dois primeiros argumentos é o range do raio de detecção dos
% circulos
[centers, radii] = imfindcircles(rgb,[30 40],'ObjectPolarity','dark');
disp('Tentativa somente raio');
disp(centers);
disp(radii);

% Porém nenhum circulo é identificado dessa forma, é necessário utilizar o
% parâmetro sensitivity
[centers, radii] = imfindcircles(rgb,[30 40],'ObjectPolarity','dark', 'Sensitivity',0.9);
disp('Tentativa com "sentivity"');
disp(centers);
disp(radii);
% Nesse caso alguns pontos são obtidos, porém diferente do exemplo
% fornecido são encontrados 5 circulos, não 8... A hipotese é de que
% tenha ocorrido alguma "alteração" na qualidade da imagem ou mudança nos
% algoritmos...

% Além disso depois é mostrado como circular as circunferencias encontradas
h = viscircles(centers,radii);

% Pode-se aumentar ainda mais a sensibilidade para encontrar novos
% circulos, sendo o limite do aumento a possibilidade dos falso negativos
[centers, radii] = imfindcircles(rgb,[30 40],'ObjectPolarity','dark', 'Sensitivity',0.9);
delete(h);
h = viscircles(centers,radii);

