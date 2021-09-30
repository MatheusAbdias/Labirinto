#ifndef _LABIRINTO_H_
#define _LABIRINTO_H_

#include <cmath>
#include <fstream>
#include <vector>

using namespace std;

#define LARGURA_MIN_MAPA 10
#define LARGURA_MED_MAPA 25
#define LARGURA_MAX_MAPA 50

#define ALTURA_MIN_MAPA 5
#define ALTURA_MED_MAPA 10
#define ALTURA_MAX_MAPA 20

#define PERC_MIN_OBST 0.05
#define PERC_MAX_OBST 0.50

/// As coordenadas de uma celula do mapa
struct Coord
{
public:
  int lin,col;

  inline Coord(): lin(-1), col(-1) {}
  inline Coord(unsigned L, unsigned C): lin(L), col(C) {}

  // Teste de validade
  inline bool valida() const {return lin>=0 && col>=0;}

  // Comparacao
  inline bool operator==(const Coord& C) const {return lin==C.lin && col==C.col;}
  inline bool operator!=(const Coord& C) const {return !operator==(C);}

  // Operacoes aritmeticas
  inline Coord operator+(const Coord& C) const {return Coord(lin+C.lin, col+C.col);}
  inline Coord operator-(const Coord& C) const {return Coord(lin-C.lin, col-C.col);}
};

// Funcoes de E/S de coordenadas
istream& operator>>(istream& I, Coord& C);
ostream& operator<<(ostream& O, const Coord& C);

// Valor absoluto de uma coordenada
inline Coord abs(const Coord& C)
{
  return Coord(abs(C.lin), abs(C.col));
}

// Norma (modulo) de uma coordenada
inline double norm(const Coord& C)
{
  return sqrt(C.lin*C.lin + C.col*C.col);
}

/// Os possiveis estados de uma celula do mapa
enum class EstadoCel
{
  LIVRE,
  OBSTACULO,
  ORIGEM,
  DESTINO,
  CAMINHO
};

// Funcao para converter um estado de celula em uma string que o represente
string estadoCel2string(EstadoCel E);

/// CLASSE Noh
class Noh
{
private:
    double g,h;
    Coord pos,ant;
public:

    ///Contrutor defaut
    inline Noh(): g(0.0),h(0.0),pos(),ant(){}

    ///Funções de retorno
    //custo total
    inline double getCusto() const {return h + g;}

    //coord da posição atual
    inline Coord getPos()const{return pos;}

    //retornar uma coord da posição anterior
    inline Coord getAnt()const{return ant;}

    //retorna o custo até chegar no noh
    inline double getG()const{return g;}

    //custo futuro
    inline double getH()const{return h;}

    ///Alteração de valor

    //altera o valor até chegar o noh
    inline void setG(const double& G){g = G;}

    //altera o valor futuro do noh
    inline void setH(const double& H){h = H;}

    //muda a pos atual recebendo uma coord
    inline void setPos(const Coord& C){pos = C;}

    //muda a posição do anterior recebendo um noh
    inline void setAnt(const Noh& N){ant = N.getPos();}

    //muda a posição do anterior recebendo uma coord
    inline void setAnt(const Coord& C){ant = C;}

    ///Operadores
    // == para noh
    inline bool operator==(const Noh& N)const {return getPos() == N.getPos();}

    //se um noh == coord
    inline bool operator==(const Coord& C)const {return getPos() == C;}

    //se um noh != uma coord
    inline bool operator!=(const Coord& C)const {return getPos()!= C;}

    //noh + coord
    inline Coord operator+(const Coord& C)const {return getPos()+ C;}

    //se um noh < noh
    inline bool operator<(const Noh &N)const {return getCusto() < N.getCusto();}

    //calculo do custo futuro
    void heuristica(const Coord& Dest);
};
/// Os elementos dos conjuntos de busca do algoritmo A*
class posi{
private:
    double custo;
public:
    inline posi(const Noh & N):custo(N.getCusto()){}
    inline bool operator()(const Noh& N)const{return custo < N.getCusto();}
};
/// A classe que armazena o mapa e os metodos de resolucao de labirintos
class Labirinto
{
private:
  /// Dimensoes do mapa
  /// NL = altura (numero de linhas)
  /// NC = largura (numero de colunas)
  unsigned int NL, NC;

  /// "Matriz" que contem os estados das casas do mapa.
  /// Na realidade, a "matriz" eh um vector de EstadoCel
  /// O acesso aos elementos da "matriz" se dah atraves dos metodos "set" e "at",
  /// que transformam os indices linha e coluna da matriz no indice do vetor
  vector<EstadoCel> mapa;

  /// A origem e o destino do caminho
  Coord orig, dest;

  /// Funcao set de alteracao de valor
  /// Retorna uma referencia ao i-j-esimo elemento do mapa
  inline EstadoCel& set(unsigned i, unsigned j) {return mapa.at(NC*i+j);}
  inline EstadoCel& set(const Coord& C) {return set(C.lin, C.col);}

public:
  /// Cria um mapa vazio
  inline Labirinto(): NL(0), NC(0), mapa(), orig(), dest() {}

  /// Torna o mapa vazio
  void clear();

  /// Limpa um eventual caminho anteriormente calculado
  void limpaCaminho();

  /// Funcoes de consulta
  inline unsigned getNumLin() const {return NL;}
  inline unsigned getNumCol() const {return NC;}
  inline Coord getOrig() const {return orig;}
  inline Coord getDest() const {return dest;}

  /// Leh um mapa do arquivo nome_arq
  /// Caso nao consiga ler do arquivo, cria mapa vazio
  /// Retorna true em caso de leitura bem sucedida
  bool ler(const string& nome_arq);
  /// Salva um mapa no arquivo nome_arq
  /// Retorna true em caso de escrita bem sucedida
  bool salvar(const string& nome_arq) const;

  /// Gera um novo mapa aleatorio
  /// numL e numC sao as dimensoes do labirinto
  /// perc_obst eh o percentual de casas ocupadas no mapa. Se <=0, assume um valor aleatorio
  /// entre PERC_MIN_OBST e PERC_MAX_OBST
  /// Se os parametros forem incorretos, gera um mapa vazio
  /// Retorna true em caso de geracao bem sucedida (parametros corretos)
  bool gerar(unsigned numL=ALTURA_MED_MAPA, unsigned numC=LARGURA_MED_MAPA,
             double perc_obst=0.0);

  /// Cria um mapa com dimensoes dadas
  /// numL e numC sao as dimensoes do labirinto
  inline Labirinto(unsigned numL, unsigned numC) {gerar(numL, numC);}

  /// Cria um mapa com o conteudo do arquivo nome_arq
  /// Caso nao consiga ler do arquivo, cria mapa vazio
  inline Labirinto(const string& nome_arq) {ler(nome_arq);}

  /// Destrutor (nao eh obrigatorio...)
  inline ~Labirinto() {}

  /// Funcao de consulta
  /// Retorna o estado da celula correspondente ao i-j-esimo elemento do mapa
  inline EstadoCel at(unsigned i, unsigned j) const {return mapa.at(NC*i+j);}
  /// Retorna o estado da celula C
  inline EstadoCel at(const Coord& C) const {return at(C.lin, C.col);}

  /// Operador() de consulta - usa o metodo "at"
  /// Retorna o estado da celula correspondente ao i-j-esimo elemento do mapa
  inline EstadoCel operator()(unsigned i, unsigned j) const {return at(i,j);}
  /// Retorna o estado da celula C
  inline EstadoCel operator()(const Coord& C) const {return at(C);}

  /// Testa se um mapa estah vazio
  inline bool empty() const {return mapa.empty();}
  /// Testa se um mapa tem origem e destino definidos
  inline bool origDestDefinidos() const {return celulaLivre(orig) && celulaLivre(dest);}

  /// Testa se uma celula eh valida dentro das dimensoes de um mapa
  bool coordValida(const Coord& C) const;
  /// Testa se uma celula estah livre (nao eh obstaculo) em um mapa
  bool celulaLivre(const Coord& C) const;
  /// Testa se um movimento Orig->Dest eh valido
  bool movimentoValido(const Coord& Orig, const Coord& Dest) const;

  /// Fixa a origem do caminho a ser encontrado
  bool setOrigem(const Coord& C);
  /// Fixa o destino do caminho a ser encontrado
  bool setDestino(const Coord& C);

  /// Imprime o mapa no console
  void imprimir() const;

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
  double calculaCaminho(int& NC, int& NA, int& NF);
};

#endif // _LABIRINTO_H_
