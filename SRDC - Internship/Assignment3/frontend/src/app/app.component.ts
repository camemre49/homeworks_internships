import {Component, OnInit} from '@angular/core';
import {Router} from "@angular/router";
import {Subscription} from "rxjs";
import {MenuService} from "./services/menu.service";
import {ApiService} from "./services/api.service";

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit{
  subscription: Subscription;
  loginSuccessful: boolean = false;

  title = 'ClientServer';
  constructor(private router:Router, private menuService: MenuService, private loginService: ApiService) {
    this.subscription = this.menuService.getLoginInfo().subscribe(value => {
      this.loginSuccessful = value;
  })
  }

  ngOnInit() {
    this.router.navigate([""]);
    if(sessionStorage.getItem("clientToken")){
      this.loginService.loginControl().subscribe((response) => {
        this.loginService.setUsername(response['username'])
        if(response["bool"]){
          this.menuService.setAdmin(true);
        }
        this.menuService.loginInfo(true);
      });
    }
  }

  turnMenu(){
    this.router.navigate([""])
  }
}
