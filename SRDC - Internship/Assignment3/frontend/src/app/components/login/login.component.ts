import { Component } from '@angular/core';
import {ApiService} from "../../services/api.service";
import {User} from "../../../User";
import {MenuService} from "../../services/menu.service";
import {Subscription} from "rxjs";


@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent {
  private users: User[] = [];
  clientToken: string;
  username: string;
  password: string;
  loginFlag: boolean = false;
  loginSuccessful: boolean;
  subscription: Subscription;

  onLogin(){
    if(!this.username){
      alert("Username cannot be empty!")
      return;
    }
    if(!this.password){
      alert("Password cannot be empty!")
      return;
    }
    sessionStorage.setItem('clientToken', "");
    let user = {
      username: this.username,
      password: this.password
    }
    this.apiService.setUsername(this.username);

    this.apiService.login(user).subscribe( (user) => {
      sessionStorage.setItem('clientToken', user["data"].username);
      this.loginFlag = true;
      this.menuService.setAdmin(user["data"].admin);
      this.menuService.loginInfo(!this.loginSuccessful);

    })

    this.username = "";
    this.password = "";
  }

  constructor(private apiService: ApiService, private menuService: MenuService) {
    this.subscription = this.menuService.getLoginInfo().subscribe((value) => {
      this.loginSuccessful = value;
    })
  }



}
