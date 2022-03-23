#include "ros/ros.h"
#include "std_msgs/Float64MultiArray.h"
#include "sensor_msgs/LaserScan.h"
#include <cstdlib> 
#include <ctime> 
#include "nav_msgs/Odometry.h"

class datos {

	protected:
	   ros::Publisher laserPub; 
	   ros::Subscriber laserSub; 
	   ros::Subscriber odometry; 

	   double maxAngle;
	   double mediumAngle;
	   double minAngle;

	public:
	   datos(ros::NodeHandle& nh) { //: rotateStartTime(ros::Time::now()), rotateDuration(0.f) {
	      maxAngle=0.0;
		   mediumAngle=0.0;
		   minAngle=0.0;
		   srand(time(NULL));
		   // Este método nos permite indicar al sistema que vamos a publicar mensajes de laserInfo
		   // El valor de 1 indica que si acumulamos varios mensajes, solo el último será enviado.
		   // El método devuelve el Publisher que recibirá los mensajes.
		   laserPub = nh.advertise<std_msgs::Float64MultiArray>("laserInfo", 1);
		   // Suscribe el método commandScan al tópico scan (el láser proporcionado por Stage)
		   // El método commandScan será llamado cada vez que el emisor (stage) publique datos 
		   laserSub = nh.subscribe("scan", 1, &datos::getScan, this);
		   odometry = nh.subscribe("odom", 1, &datos::getOdom, this); 
	   };

	   //Procesa los datos de Odemetria
	   void getOdom(const nav_msgs::Odometry::ConstPtr& msg) {
          double posX =  msg->pose.pose.position.x;
          double posY =  msg->pose.pose.position.y;
          double angZ = 2*atan2(msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);

          std::cout << "POSICION X: " << posX << " POSICION Y: " << posY << std::endl;
          std::cout << "ANGZ: " << angZ << std::endl;
          std::cout << "--------------------------------------------" << std::endl;

	   }

	   //Procesa los datos del laser
	   void getScan(const sensor_msgs::LaserScan::ConstPtr& msg) {
         std::cout << "AngleMin: " << msg->angle_min << std::endl; // Mínimo valor angular del láser
	      std::cout << "AngleMax: " << msg->angle_max << std::endl; // Máximo valor angular del láser
	      std::cout << "AngleIncrement: " << msg->angle_increment << std::endl; // Incremento angular entre dos beams
		   std::cout << "RangeMin: " << msg->range_min << std::endl; // Mínimo valor que devuelve el láser
		   std::cout << "RangeMax: " << msg->range_max << std::endl; // Máximo valor que devuelve el láser. Valores por debajo y por encima de estos rangos no deben ser tenidos en cuenta.
		  
		   int totalValues = ceil((msg->angle_max-msg->angle_min)/msg->angle_increment); // Total de valores que devuelve el láser (359)
		   std::cout << "Vista frontal (0º): " << msg->ranges[0] << std::endl;
		   std::cout << "Vista trasera (160º): " << msg->ranges[160] << std::endl;
		   std::cout << "Vista derecha (320º): " << msg->ranges[320] << std::endl;

		   minAngle = msg->ranges[0];
		   mediumAngle = msg->ranges[160];
		   maxAngle = msg->ranges[320];
	   }


	   //Bucle principal
	   void bucle() {
	      ros::Rate rate(10); // Especifica el tiempo de bucle en Hertzios. Ahora está en ciclo por segundo, pero normalmente usaremos un valor de 10 (un ciclo cada 100ms).
		   while (ros::ok()) { // Bucle que estaremos ejecutando hasta que paremos este nodo o el roscore pare.
		      std_msgs::Float64MultiArray msg; // Almacenamos mensajes de los ranges[i]

		      msg.data.push_back(minAngle);
		      msg.data.push_back(mediumAngle);
		      msg.data.push_back(maxAngle);

		      laserPub.publish(msg); // Publicamos el mensaje en el topico laserInfo		 
					
			   ros::spinOnce(); // Se procesarán todas las llamadas pendientes, es decir, llamará a callBack
			   rate.sleep(); // Espera a que finalice el ciclo
	      }
	   }
};

int main(int argc, char **argv) {
   ros::init(argc, argv, "datos"); // Inicializa un nuevo nodo llamado datos
   ros::NodeHandle nh;
   datos dat(nh); // Crea un objeto de esta clase y lo asocia con roscore
   dat.bucle(); // Ejecuta el bucle
   return 0;
};