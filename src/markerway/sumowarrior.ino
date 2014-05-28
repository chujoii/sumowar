/*
  sumowarrior.ino --- fight program for robot

  Copyright (C) 2014 Roman V. Prikhodchenko



  Author: Roman V. Prikhodchenko <chujoii@gmail.com>


  This file is part of markerway.

    markerway is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    markerway is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with markerway.  If not, see <http://www.gnu.org/licenses/>.



 Keywords: robot sumo



 Usage:


 History:
 

 Code:
*/


void sumowarrior(){
	/*
   // найдём ближайшую цель - в реальности не сработает потому, что ближайшей целью скорее всего окажется нога зрителя
   int enemy_angle = min_index(ir_hi, ir_hi_dimension) * ir_hi_angle; // номер минимального элемента умножим на шаг в градусах получим угол расположения противника
   int speed = 255;
   int counter; // счётчик в цикле

   boolean all_white = true; //

   if (button_sensor[0] || button_sensor[1]){
      // он попался! раз коснулся переднего отвала
      bulldozer_blade(255); // не забудьте засечь время и выключить через ~10 секунд!
   }
   if (button_sensor[5] || button_sensor[6]){
      // он попался! раз коснулся задней переворачивалки
      bulldozer_turn(255); // не забудьте засечь время и выключить через ~10 секунд!
   }
   
   
   for (counter = 0; counter< line_sensor_dimension; counter++){
      // в цикле проверяем датчики линии под колёсами
      if (itisblack(line_sensor[counter])) { // если не дотягивает до белого, то оно чёрное
         all_white = false;
      }
   }   

   if (all_white) {
      // если все датчики линии показывают белый свет, то разворачиваемся (толкать лучше отвалом)
      // но закомментировано потому что одновременное движение и вращение описываются другими формулами (Трохоида - например удлинённая циклоида)
      // вы сперва повернитесь (например 3 секунды и только потом двигайтесь)
      // set_wheels_rotate (0, 0, 10); // 10 маленькая скорость
      

      // толкаем противника
      set_wheels_straight (enemy_angle, 255);


      // проверяйте лимит времени в 30 секунд:
      // если одно и тоже действие длится больше 30 секунд (проверить регламент), то засчитывается поражение! (либо ничья)
   } else {
      // раз коснулись линии то мы вытолкали противника и значит останавливаемся (скорость = 0)
      set_wheels_straight (enemy_angle, 0);
   }


   delay(cycle_delay_ms); // небольшая задержка нужна!

	*/
}

