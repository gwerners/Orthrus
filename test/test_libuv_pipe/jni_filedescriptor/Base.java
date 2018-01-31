import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileDescriptor;


public class Base {


   private FileInputStream in, out;

   public static void main(String[] args)
   {
     System.loadLibrary("example");

      BufferedInputStream bis = null;
      byte[] buffer = new byte[8192];
      int bytesRead = 0;
      FileDescriptor myFD = new FileDescriptor();

      sun.misc.SharedSecrets.getJavaIOFileDescriptorAccess().set(myFD,example.GetDescriptor());
      //
      try {
         bis = new BufferedInputStream(new FileInputStream(myFD));
         while ((bytesRead = bis.read(buffer)) != -1) {
            System.out.write(buffer, /* start */ 0, /* length */ bytesRead);
         }
      } catch (Exception e) {
         e.printStackTrace();
      } finally {
         try {
            bis.close();
         } catch (Exception e) { /* meh */ }
      }
      //

   }
}
