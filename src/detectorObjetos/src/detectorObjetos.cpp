#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
#include <cstdlib> 
#include <ctime> 
#include "nav_msgs/Odometry.h"
#include <math.h>

class DetectorObjetos {
   protected:
     ros::Publisher velPub; 
	  ros::Subscriber laserSub;

	  double range1, range2, range3, range4, range5;
	  double velLinear = 0.13, rotateVel = 0.0;

   public:
      DetectorObjetos(ros::NodeHandle& nh) { //: rotateStartTime(ros::Time::now()), rotateDuration(0.f) {
		 srand(time(NULL));
		 // Este método nos permite indicar al sistema que vamos a publicar mensajes de cmd_vel
		 // El valor de 1 indica que si acumulamos varios mensajes, solo el último será enviado.
		 // El método devuelve el Publisher que recibirá los mensajes.
		 velPub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1);
		 // Suscribe el método getScanData al tópico base_scan (el láser proporcionado por Stage)
		 // El método getScanData será llamado cada vez que el emisor (stage) publique datos 
		 laserSub = nh.subscribe("scan", 1, &DetectorObjetos::getScanData, this);
      };

      
      //Obtenemos los datos del LaserScan y paramos al turtlebot cuando detectamos un objeto delante
      void getScanData(const sensor_msgs::LaserScan::ConstPtr& msg) {
         range1 = msg->ranges[0];
         range2 = msg->ranges[3];
         range3 = msg->ranges[6];
         range4 = msg->ranges[354];
         range5 = msg->ranges[357];
         
         if(velLinear > 0.0) {
            std::cout << "Vista frontal(0º): " << msg->ranges[0] << std::endl;
            std::cout << "Vista 3º: " << msg->ranges[3] << std::endl;
            std::cout << "Vista 6º: " << msg->ranges[6] << std::endl;
            std::cout << "Vista 354º: " << msg->ranges[354] << std::endl;
            std::cout << "Vista 357º: " << msg->ranges[357] << std::endl;
            std::cout << std::endl;
         }


         if(range1 > 0.19 && range1 < 0.46 || range2 > 0.25 && range2 < 0.42 ||
         	range3 > 0.25 && range3 < 0.42 || range4 > 0.25 && range4 < 0.42 ||
         	range5 > 0.25 && range5 < 0.42) {
            
            velLinear = 0.0; // El robot se para al detectar a un objeto
            rotateVel = 0.0;
            std::cout << "ROBOT PARADO" << std::endl;
            //if(isnan(range1)) { std::cout << "NAeererererANAN" << std::endl; }

         }
      }

      // Bucle principal
	  void bucle() {
	    ros::Rate rate(10); // Especifica el tiempo de bucle en Hertzios. Ahora está en ciclo por segundo, pero normalmente usaremos un valor de 10 (un ciclo cada 100ms).
		 while (ros::ok()) { // Bucle que estaremos ejecutando hasta que paremos este nodo o el roscore pare.
		 	
		   geometry_msgs::Twist msg; // Este mensaje está compuesto por dos componentes: linear y angular. Permite especificar dichas velocidades
						  //  Cada componente tiene tres posibles valores: x, y, z, para cada componente de la velocidad. En el caso de
						  // robots que reciben velocidad linear y angular, debemos especificar la x linear y la z angular.

			msg.linear.x = msg.linear.y = msg.linear.z = velLinear;
			msg.angular.z = msg.angular.x = msg.angular.y = rotateVel;
			velPub.publish(msg);
			//if(velLinear == 0.0) { return; }		
			ros::spinOnce(); // Se procesarán todas las llamadas pendientes, es decir, llamará a getScanData
			rate.sleep(); // Espera a que finalice el ciclo
		 }
	  }
};

int main(int argc, char **argv) {
   ros::init(argc, argv, "detector"); // Inicializa un nuevo nodo llamado wander
	ros::NodeHandle nh;
	DetectorObjetos detector(nh); // Crea un objeto de esta clase y lo asocia con roscore
	detector.bucle(); // Ejecuta el bucle
	return 0;
};
