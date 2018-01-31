
public class Base {

   public static void main(String argv[]) {
     System.loadLibrary("example");
     System.out.println(example.getMy_variable());
     System.out.println(example.fact(5));
     System.out.println(example.get_time());
   }
 }

