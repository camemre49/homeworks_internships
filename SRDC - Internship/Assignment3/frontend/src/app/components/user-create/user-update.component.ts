import {Component, Input} from '@angular/core';
import {ApiService} from "../../services/api.service";
import {User} from "../../../User";
import {MenuService} from "../../services/menu.service";
import {Output, EventEmitter} from "@angular/core";

@Component({
  selector: 'app-user-update',
  templateUrl: './user-update.component.html',
  styleUrls: ['./user-update.component.css']
})
export class UserUpdateComponent {
  @Output() emitter = new EventEmitter<User>;
  username: string;
  password: string;
  name: string;
  surname: string;
  email: string;
  city: string;
  gender: string;
  birthDate: string;
  admin: string;
  @Input() createOrUpdate: string = "create";
  @Input() userId: string;
  updateUser(){
    if(this.createOrUpdate === "create"){
      if(!this.username){
        alert("Username field cannot be empty!")
        return;
      }
      if(!this.password){
        alert("Password field cannot be empty!")
        return
      }
      if(!this.email){
        alert("Email field cannot be empty!")
        return
      }
      if(!this.admin){
        this.admin = "false";
      }
    }
    let birth;
    if (this.birthDate) {
      const dateParts = this.birthDate.split('-');
      const year = +dateParts[0];
      const month = +dateParts[1] - 1;
      const day = +dateParts[2] + 1;
      birth = new Date(year, month, day)
    }

    let user: User = {
      username: this.username,
      password: this.password,
      name: this.name,
      surname: this.surname,
      email: this.email,
      city: this.city,
      gender: this.gender,
      dateOfBirth: birth,
      admin: this.admin
    }

    let u = this.username
    if(this.createOrUpdate === "create"){
      this.loginService.create(user).subscribe((data) =>{
        alert("User " + u + " created successfully.");
      })
      this.menuService.turnMenu();
    }
    else{
      user._id = this.userId;
      this.loginService.update(user).subscribe((response) =>{
        alert("User " + response["data"]["username"] + " updated successfully.");
        this.emitter.emit(response["data"]);
      })

      this.username = "";
      this.password = "";
      this.name = "";
      this.surname = "";
      this.email = "";
      this.city = "";
      this.gender = "";
      this.birthDate = null;
      this.admin = "";
    }
  }

  constructor(private loginService:ApiService, private menuService: MenuService) {
  }



}
