package Modelo.Metodos_Matrizes

import Modelo.Matriz.Matriz

//interface para implementar metodos de potenciacao
trait Metodo {

  def apply(A: Matriz , x0: Matriz , erro: Double): (Double,Matriz)

}
