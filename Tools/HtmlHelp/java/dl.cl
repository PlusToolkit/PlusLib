����  - � Code 
SourceFile ConstantValue 
Exceptions DialogLayout  java/lang/Object  java/awt/Dimension 	 I m_width  	   m_height  	   (II)V <init>  
 
  ()V  
   java/util/Hashtable 
   Ljava/util/Hashtable; m_map  	   (Ljava/awt/Container;II)V 	Construct !  
  " width $ 	 
 % height ' 	 
 ( x *  java/awt/Rectangle ,	 - + y / 	 - 0	 - %	 - ( (IIII)V  4
 - 5 8(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object; put 8 7
  9 &(Ljava/lang/Object;)Ljava/lang/Object; get < ;
  = ()Ljava/awt/Font; getFont @ ? java/awt/Component B
 C A '(Ljava/awt/Font;)Ljava/awt/FontMetrics; getFontMetrics F E
 C G ()I 	getHeight J I java/awt/FontMetrics L
 M K countComponents O I java/awt/Container Q
 R P
 -  (Ljava/awt/Container;)I getCharHeight V U
  W getCharWidth Y U
  Z ()Ljava/awt/Insets; insets ] \
 R ^ (I)Ljava/awt/Component; getComponent a `
 R b (Ljava/awt/Rectangle;II)V mapRectangle e d
  f java/awt/Label h java.vendor j &(Ljava/lang/String;)Ljava/lang/String; getProperty m l java/lang/System o
 p n 	Microsoft r (Ljava/lang/String;)I indexOf u t java/lang/String w
 x v left z  java/awt/Insets |	 } { top  	 } � reshape � 4
 C � 4abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ � stringWidth � t
 M � length � I
 x � java/awt/LayoutManager � DialogLayout.java addLayoutComponent )(Ljava/lang/String;Ljava/awt/Component;)V removeLayoutComponent (Ljava/awt/Component;)V preferredLayoutSize *(Ljava/awt/Container;)Ljava/awt/Dimension; minimumLayoutSize layoutContainer (Ljava/awt/Container;)V +(Ljava/awt/Container;Ljava/awt/Dimension;)V setShape (Ljava/awt/Component;IIII)V +(Ljava/awt/Component;Ljava/awt/Rectangle;)V getShape *(Ljava/awt/Component;)Ljava/awt/Rectangle; getDialogSize ()Ljava/awt/Dimension; unmapRectangle     �                   � �          � 
Y*� *� � �             #     *� *� Y� � *+� #�       �     )     *� *� Y� � *+,� &,� )� #�      � d     ?     3**� .hl� .**� 1hl� 1**� 2hl� 2**� 3hl� 3�      � �     #     *� +� -Y� 6� :W�      � �     -     !*� +� -Y,� .,� 1,� 2,� 3� 6� :W�      � �     0     $*� +� >� -M� -Y,� .,� 1,� 2,� 3� 6�      � �          � 
Y*� *� � �      � �           �      e d     ?     3**� .hl� .**� 1hl� 1**� 2hl� 2**� 3hl� 3�      � �          � 
Y*� *� � �      V U          **� D� HL+� N=�      � �     �  
   �+� S=� -Y� TN+� X6+� [6+� _:++� D� HW6� �+� c:*� � >� -:		� �-	� .� .-	� 1� 1-	� 3� 3-	� 2� 2-� g� i� 'k� qs� y� -Y� .d� .-Y� 2`� 2-Y� .� ~`� .-Y� 1� �`� 1-� .-� 1-� 2-� 3� ����V�      � �           �      Y U     +     **� D� HL�M+,� �,� �l>� >�      !       9     -� -Y� 6:+� [+� X� g*� 2� *� 3� �          �