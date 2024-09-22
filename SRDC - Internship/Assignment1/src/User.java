
public class User {
    private String username;
    private String password;
    private String name;
    private String surname;
    private String gender;
    private String email;
    private String birthdate;
    private String city;
    private String admin;

    private int index;

    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }

    public String getName() {
        return name;
    }

    public String getSurname() {
        return surname;
    }

    public String getGender() {
        return gender;
    }

    public String getEmail() {
        return email;
    }

    public String getBirthdate() {
        return birthdate;
    }

    public String getCity() {
        return city;
    }

    public String getAdmin(){
        return admin;
    }


    public int getIndex(){
        return  index;
    }

    public void setUserneme(String usern){
        username = usern;
    }

    public User(String username, String password, String name, String surname, String gender,
                String email, String city, String birthdate, String admin) {
        this.username = username;
        this.password = password;
        this.name = name;
        this.surname = surname;
        this.gender = gender;
        this.email = email;
        this.birthdate = birthdate;
        this.city = city;
        this.admin = admin;
    }

    public User() {
        this("", "", "", "", "", "", "", "", "");
    }

    public String concatenateUserFields() { // Yerini değiştir
        String[] fields = {
                this.getUsername(),
                this.getPassword(),
                this.getName(),
                this.getSurname(),
                this.getGender(),
                this.getEmail(),
                this.getCity(),
                this.getBirthdate(),
                this.getAdmin()
        };
        return String.join(":::", fields);
    }

    public void setIndex(int index) {
        this.index = index;
    }
}