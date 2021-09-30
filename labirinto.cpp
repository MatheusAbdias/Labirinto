#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>   /* srand, rand */
#include <string>
#include <time.h>
#include <cmath>
#include <list>
#include <algorithm>

#include "labirinto.h"

using namespace std;

/* ***************** */
/* CLASSE CELULA     */
/* ***************** */
string estadoCel2string(EstadoCel E)
{
  switch(E)
  {
  case EstadoCel::LIVRE:
    return "  ";
  case EstadoCel::OBSTACULO:
    return "##";
  case EstadoCel::ORIGEM:
    return "Or";
  case EstadoCel::DESTINO:
    return "De";
  case EstadoCel::CAMINHO:
    return "..";
  default:
    break;
  }
  return "??";
}

istream& operator>>(istream& I, Coord& C)
{
  I >> C.lin >> C.col;
  return I;
}

ostream& operator<<(ostream& O, const Coord& C)
{
  O << C.lin << ';' << C.col;
  return O;
}

/* ***************** */
/* CLASSE NOH        */
/* ***************** */

 void Noh::heuristica(const Coord& Dest){
    double deltx,delty;
    //variação em x
    deltx = abs(Dest.lin - pos.lin);
    //variação em y
    delty = abs(Dest.col - pos.col);
    //altera o custo furturo
    h = sqrt(2)*min(deltx , delty) + abs(deltx - delty);

 }

/* ***************** */
/* CLASSE LABIRINTO  */
/* ***************** */

/// Torna o mapa vazio
void Labirinto::clear()
{
  // Esvazia o mapa de qualquer conteudo anterior
  NL = NC = 0;
  mapa.clear();
  // Apaga a origem e destino do caminho
  orig = dest = Coord();
}

/// Limpa o caminho anterior
void Labirinto::limpaCaminho()
{
  if (!empty()) for (unsigned i=0; i<NL; i++) for (unsigned j=0; j<NC; j++)
  {
    if (at(i,j) == EstadoCel::CAMINHO)
    {
      set(i,j) = EstadoCel::LIVRE;
    }
  }
}

/// Leh um mapa do arquivo nome_arq
/// Caso nao consiga ler do arquivo, cria mapa vazio
/// Retorna true em caso de leitura bem sucedida
bool Labirinto::ler(const string& nome_arq)
{
  // Limpa o mapa
  clear();

  // Abre o arquivo
  ifstream arq(nome_arq.c_str());
  if (!arq.is_open())
  {
    return false;
  }

  string prov;
  int numL, numC;
  int valor;

  // Leh o cabecalho
  arq >> prov >> numL >> numC;
  if (prov != "LABIRINTO" ||
      numL<ALTURA_MIN_MAPA || numL>ALTURA_MAX_MAPA ||
      numC<LARGURA_MIN_MAPA || numC>LARGURA_MAX_MAPA)
  {
    arq.close();
    return false;
  }

  // Redimensiona o mapa
  NL = numL;
  NC = numC;
  mapa.resize(NL*NC);

  // Leh as celulas do arquivo
  for (unsigned i=0; i<NL; i++) for (unsigned j=0; j<NC; j++)
  {
    arq >> valor;
    set(i,j) = (valor==0 ?
                EstadoCel::OBSTACULO :
                EstadoCel::LIVRE);
  }
  arq.close();
  return true;
}

/// Salva um mapa no arquivo nome_arq
/// Retorna true em caso de escrita bem sucedida
bool Labirinto::salvar(const string& nome_arq) const
{
  // Testa o mapa
  if (empty()) return false;

  // Abre o arquivo
  ofstream arq(nome_arq.c_str());
  if (!arq.is_open())
  {
    return false;
  }

  // Salva o cabecalho
  arq << "LABIRINTO " << NL << ' ' << NC << endl;

  // Salva as celulas do mapa
  for (unsigned i=0; i<NL; i++)
  {
    for (unsigned j=0; j<NC; j++)
    {
      arq << (at(i,j) == EstadoCel::OBSTACULO ? 0 : 1 ) << ' ';
    }
    arq << endl;
  }

  arq.close();
  return true;
}

/// Gera um novo mapa aleatorio
/// numL e numC sao as dimensoes do labirinto
/// perc_obst eh o percentual de casas ocupadas no mapa. Se <=0, assume um valor aleatorio
/// entre PERC_MIN_OBST e PERC_MAX_OBST
/// Se os parametros forem incorretos, gera um mapa vazio
/// Retorna true em caso de geracao bem sucedida (parametros corretos)
bool Labirinto::gerar(unsigned numL, unsigned numC, double perc_obst)
{
  // Limpa o mapa
  clear();

  // Inicializa a semente de geracao de numeros aleatorios
  srand(time(NULL));

  // Calcula o percentual de obstaculos no mapa
  if (perc_obst <= 0.0)
  {
    perc_obst = PERC_MIN_OBST +
                (PERC_MAX_OBST-PERC_MIN_OBST)*(rand()/double(RAND_MAX));
  }

  // Testa os parametros
  if (numL<ALTURA_MIN_MAPA || numL>ALTURA_MAX_MAPA ||
      numC<LARGURA_MIN_MAPA || numC>LARGURA_MAX_MAPA ||
      perc_obst<PERC_MIN_OBST || perc_obst>PERC_MAX_OBST)
  {
    return false;
  }

  // Assume as dimensoes passadas como parametro
  NL = numL;
  NC = numC;

  // Redimensiona o mapa
  mapa.resize(NL*NC);

  // Preenche o mapa
  bool obstaculo;
  for (unsigned i=0; i<NL; i++) for (unsigned j=0; j<NC; j++)
  {
    obstaculo = (rand()/double(RAND_MAX) <= perc_obst);
    set(i,j) = (obstaculo ?
                EstadoCel::OBSTACULO :
                EstadoCel::LIVRE);
  }
  return true;
}

/// Testa se uma celula eh valida dentro de um mapa
bool Labirinto::coordValida(const Coord& C) const
{
  if (!C.valida()) return false;
  if (C.lin >= int(NL)) return false;
  if (C.col >= int(NC)) return false;
  return true;
}

/// Testa se uma celula estah livre (nao eh obstaculo) em um mapa
bool Labirinto::celulaLivre(const Coord& C) const
{
  if (!coordValida(C)) return false;
  if (at(C) == EstadoCel::OBSTACULO) return false;
  return true;
}

/// Testa se um movimento Orig->Dest eh valido
bool Labirinto::movimentoValido(const Coord& Orig, const Coord& Dest) const
{
  // Soh pode mover de e para celulas livres
  if (!celulaLivre(Orig)) return false;
  if (!celulaLivre(Dest)) return false;

  // Soh pode mover para celulas vizinhas
  Coord delta=abs(Dest-Orig);
  if (delta.lin>1 || delta.col>1) return false;

  // Nao pode mover em diagonal se colidir com alguma quina
  // Se o movimento nao for diagonal, esses testes sempre dao certo,
  // pois jah testou que Orig e Dest estao livres e ou a linha ou a
  // coluna de Orig e Dest sao iguais
  if (!celulaLivre(Coord(Orig.lin,Dest.col))) return false;
  if (!celulaLivre(Coord(Dest.lin,Orig.col))) return false;

  // Movimento valido
  return true;
}

/// Fixa a origem do caminho a ser encontrado
bool Labirinto::setOrigem(const Coord& C)
{
  if (!celulaLivre(C)) return false;
  // Se for a mesma origen nao faz nada
  if (C==orig) return true;

  limpaCaminho();

  // Apaga a origem anterior no mapa, caso esteja definida
  if (coordValida(orig)) set(orig) = EstadoCel::LIVRE;

  // Fixa a nova origem
  orig = C;
  // Marca a nova origem no mapa
  set(orig) = EstadoCel::ORIGEM;

  return true;
}

/// Fixa o destino do caminho a ser encontrado
bool Labirinto::setDestino(const Coord& C)
{
  if (!celulaLivre(C)) return false;
  // Se for o mesmo destino nao faz nada
  if (C==dest) return true;

  limpaCaminho();

  // Apaga o destino anterior no mapa, caso esteja definido
  if (coordValida(dest)) set(dest) = EstadoCel::LIVRE;

  // Fixa o novo destino
  dest = C;
  // Marca o novo destino no mapa
  set(dest) = EstadoCel::DESTINO;

  return true;
}

/// Imprime o mapa no console
void Labirinto::imprimir() const
{
  if (empty())
  {
    cout << "+------------+" << endl;
    cout << "| MAPA VAZIO |" << endl;
    cout << "+------------+" << endl;
    return;
  }

  unsigned i,j;

  // Impressao do cabecalho
  cout << "    ";
  for (j=0; j<NC; j++)
  {
    cout << setfill('0') << setw(2) << j << setfill(' ') << setw(0) << ' ' ;
  }
  cout << endl;

  cout << "   +";
  for (j=0; j<NC; j++) cout << "--+" ;
  cout << endl;

  // Imprime as linhas
  for (i=0; i<NL; i++)
  {
    cout << setfill('0') << setw(2) << i << setfill(' ') << setw(0) << " |" ;
    for (j=0; j<NC; j++)
    {
      cout << estadoCel2string(at(i,j)) << '|' ;
    }
    cout << endl;

    cout << "   +";
    for (j=0; j<NC; j++) cout << "--+" ;
    cout << endl;
  }
}

/// Calcula o caminho entre a origem e o destino do labirinto usando o algoritmo A*
///
/// Retorna o comprimento do caminho (<0 se nao existe)
///
/// O parametro NC retorna o numero de nos no caminho encontrado (profundidade da busca)
/// O parametro NC retorna <0 caso nao exista caminho.
///
/// O parametro NA retorna o numero de nos em aberto ao termino do algoritmo A*
/// O parametro NF retorna o numero de nos em fechado ao termino do algoritmo A*
/// Mesmo quando nao existe caminho, esses parametros devem ser retornados
double Labirinto::calculaCaminho(int& NC, int& NA, int& NF)
{
  if (empty() || !origDestDefinidos())
  {
    // Impossivel executar o algoritmo
    NC = NA = NF = -1;
    return -1.0;
  }

  // Apaga um eventual caminho anterior
  limpaCaminho();

  // Testa se origem igual a destino
  if (orig == dest)
  {
    // Caminho tem profundidade nula
    NC = 0;
    // Algoritmo de busca nao gerou nenhum noh
    NA = NF = 0;
    // Caminho tem comprimento nulo
    return 0.0;
  }

  list<Noh> Aberto;
  list<Noh> Fechado;
  list<Noh>::iterator oldA;
  list<Noh>::iterator oldF;
  list<Noh>::iterator pos;

  //noh inicilizado por default
  Noh atual;

  //atual recebe a posição da origem
  atual.setPos(orig);

  //calculo a distancia até o destino
  atual.heuristica(dest);

  //variaveis auxiliares
  Coord dir,prox;
  Noh suc;

  //inicializa o conjunto aberto com atual
  Aberto.push_front(atual);
  do{

    //noh a ser analizado é sempre o da frente de aberto
    atual = Aberto.front();

    //remove o primeiro elemento
    Aberto.pop_front();

    //insere o noh atual em fechado
    Fechado.push_front(atual);

    //Se o noh atual for diferente do destinho
    if(atual != dest){
        //gerar sucessores
        for(dir.lin = -1; dir.lin <= 1; dir.lin++){
            for( dir.col = -1; dir.col <= 1 ; dir.col++){

                // se o sucessor n for nulo
                if(dir != Coord(0,0)){

                    // soma de um noh com uma coord
                    prox = atual + dir;

                    //testo se o movimento é valido se sim crio o sucessor
                    if(movimentoValido(atual.getPos(),prox)){

                        //posição do sucessor
                        suc.setPos(prox);

                        //posição do anterior
                        suc.setAnt(atual);

                        //custo vai g vai ser a soma do custo passado mais o custo do movimento
                        suc.setG(atual.getG() + norm(dir));

                        //calcula o custo futuro do sucessor
                        suc.heuristica(dest);

                        //procuro se o noh ja foi visitado usando ==  entre noh
                        oldF = find(Fechado.begin() , Fechado.end(), suc);

                        //se o noh foi encontrado
                        if(oldF != Fechado.end()){

                            // pergunto se o noh em fechado é melhor
                            if(suc < *oldF){
                                Fechado.erase(oldF);
                                //faço o interado apontar para o final de fechado
                                oldF = Fechado.end();
                            }
                        }
                        else{
                            //usa o == para noh
                            oldA = find(Aberto.begin() , Aberto.end(), suc);

                            //Se o noh encontrado em aberto
                            if(oldA != Aberto.end()){

                                //pergunto se o sucessor em aberto é melhor
                                if(suc < *oldA){
                                    //se não removo o noh de aberto
                                    Aberto.erase(oldA);
                                    //faço apontar para o final da list
                                    oldA = Aberto.end();
                                }
                            }
                        }
                        //se não foi passado por esse suc ainda
                        if(oldF == Fechado.end() && oldA == Aberto.end()){
                            pos = find_if( Aberto.begin(), Aberto.end(),posi(suc));
                            Aberto.insert(pos,suc);
                        }
                    }
                }
            }
        }
    }
}while(atual.getPos() != dest && !Aberto.empty());


    if(atual.getPos() != getDest()){
        // Erro no calculo do caminho
        NC = NA = NF = -1;
        return -1.0;
    }
    NF = Fechado.size();
    NA = Aberto.size();
    double profundidade;
    double comprimento = atual.getG();
    profundidade = 1;
    while(atual.getAnt() != orig){
        //procuro antecessor em fechado
        set(atual.getAnt())= EstadoCel::CAMINHO;
        oldF = find(Fechado.begin(),Fechado.end(),atual.getAnt());
        atual = *oldF;
        profundidade++;
    }
    NC = profundidade;
    return(comprimento);
}
